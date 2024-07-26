
#include "WsStickyWrap.h"

#include <assert.h>
#include <string.h>

#include <cstring>

#include "../common/FdBridge.h"
#include "../common/sha1.h"
#include "../common/ws.h"
#include "../memoryBuff/memoryBuff.h"

WsStickyWrap::WsStickyWrap(int fd) : FdBridge(fd) {}

WsStickyWrap::~WsStickyWrap() {}

int WsStickyWrap::readData(char *pbuff, uint16_t len) {
  int realLen = 0;
  if (len > 0) {
    if (len < _Data_cap - _Data_size) {
      memcpy(_pData + _Data_size, pbuff, len);
      _Data_size += len;
      realLen += len;
      int read_len = 0;
      do {
        int parse_len =
            parseWsHead((char *)(_pData + _Read_size), _Data_size - _Read_size);
        if (parse_len < 0) break;
        _Read_size += parse_len;
        read_len += parse_len;
        if (_head.wbs->fin && _parse_Index && _parse_Index >= _head.len) {
          // 一个帧完成
          unsigned short headsize = CalFrameHeadSize();
          switch (_head.wbs->opcod) {
            case WS_FR_OP_CLSE:
              if (sendFrame((char *)_pData + headsize, _Data_size - headsize,
                            WS_FR_OP_CLSE) < 0) {
                return -1;
              }
              break;
            case WS_FR_OP_PING:
              if (sendFrame((char *)_pData + headsize, _Data_size - headsize,
                            WS_FR_OP_PING) < 0) {
                return -1;
              }
              break;
            case WS_FR_OP_PONG:
              // 应该不用处理
              break;
            case WS_FR_OP_UNSUPPORTED:
              break;
            default:
              if (_head.len >= _Data_size - headsize) {
                finishParse((char *)_pData + headsize, _head.len);
                memcpy(_pData, _pData + headsize + _head.len,
                       _Data_size - headsize - _head.len);
                _Data_size = _Data_size - headsize - _head.len;
                _Read_size = 0;
              } else {
                return -1;
              }
              break;
          }
        }
      } while (_Data_size - _Read_size > 0);
    } else {
      printf("long data\n");
      return -1;
    }
  } else {
    printf("not read data\n");
  }

  return realLen;
}
int WsStickyWrap::parseWsHead(char *pbuff, int len) {
  int readlen = 0;
  if (len > 0) {
    switch (_Op) {
      case OP_State::INIT_HEAD:
        if (len >= sizeof(ws_base)) {
          _head.wbs = (ws_base *)pbuff;
          readlen += _headSize;
          _head.len = 0;
          memset(_head.masks, 0, sizeof(_head.masks));
          _Op = OP_State::READ_PAYLOAD;
          pbuff += _headSize;
          _parse_Index = 0;
        }
        break;
      case OP_State::READ_PAYLOAD:
        if (_head.wbs->payLoad < 126) {
          _head.len = _head.wbs->payLoad;
          _Op = OP_State::READ_MASK_KEY;
        } else if (_head.wbs->payLoad == 126 && len - readlen >= 2) {
          _head.len = (((uint64_t)pbuff[0]) << 8) | pbuff[1];
          readlen += 2;
          pbuff += 2;
          _Op = OP_State::READ_MASK_KEY;
        } else if (_head.wbs->payLoad == 127 && len - readlen >= 8) {
          _head.len =
              (((uint64_t)pbuff[0]) << 56) | (((uint64_t)pbuff[1]) << 48) |
              (((uint64_t)pbuff[2]) << 40) | (((uint64_t)pbuff[3]) << 32) |
              (((uint64_t)pbuff[4]) << 24) | (((uint64_t)pbuff[5]) << 16) |
              (((uint64_t)pbuff[6]) << 8) | (((uint64_t)pbuff[7]));
          readlen += 8;
          pbuff += 8;
          _Op = OP_State::READ_MASK_KEY;
        }
        break;
      case OP_State::READ_MASK_KEY:
        if (len - readlen >= 4) {
          _head.masks[0] = pbuff[0];
          _head.masks[1] = pbuff[1];
          _head.masks[2] = pbuff[2];
          _head.masks[3] = pbuff[3];
          readlen += 4;
          pbuff += 4;
          _Op = OP_State::READ_DATA;
        }
        break;
      case OP_State::READ_DATA: {
        while (_parse_Index < _head.len && readlen < len) {
          char cur_byte = *pbuff;
          if (cur_byte == -1) return (-1);
          *pbuff = cur_byte ^ _head.masks[_parse_Index % 4];
          pbuff++;
          _parse_Index++;
          readlen++;
        }
        if (_parse_Index >= _head.len) {
          _Op = OP_State::INIT_HEAD;
        }
      } break;
      default:
        readlen = -1;
        break;
    }
  }
  return readlen;
}
int WsStickyWrap::finishParse(char *pData, int size) {
  int realLen = 0;
  while (true) {
    int readlen = 0;
    if (_pack._Data_size < _pack._headSize) {
      size_t cpsize = size > _pack._headSize - _pack._Data_size
                          ? _pack._headSize - _pack._Data_size
                          : size;
      unsigned char *pWrite = _pack._pData + _pack._Data_size;
      memcpy(pWrite, pData, cpsize);
      readlen += cpsize;
      _pack._Data_size += cpsize;
    }
    if (_pack._Data_size >= _pack._headSize &&
        _pack._head->_len >
            _pack._Data_cap)  // 数据超过一个包的数据，认为是非法数据 需要close
    {
      return -1;
    }
    if (_pack._head->_len >= _pack._Data_size + (size - readlen)) {
      memcpy(_pack._pData + _pack._Data_size, pData + readlen,
             (size - readlen));
      _pack._Data_size += size - readlen;
      readlen = size;
    } else {
      memcpy(_pack._pData + _pack._Data_size, pData + readlen,
             _pack._head->_len - _pack._Data_size);
      readlen += _pack._head->_len - _Data_size;
      _Data_size += _pack._head->_len - _Data_size;
    }
    realLen += readlen;
    if (_pack._Data_size > _pack._headSize && _pack._head->_len == _Data_size) {
      // 一个包体完成
      MakeServerMsg(_pack._pData,_pack._head->_msgId, _pack._head->_len);
      _Data_size = 0;
    }
  }

  return realLen;
}

unsigned short WsStickyWrap::CalFrameHeadSize() {
  unsigned short len = sizeof(ws_base) + 4;
  if (_head.wbs->payLoad == 126) {
    len += 2;
  } else if (_head.wbs->payLoad == 127) {
    len += 8;
  }
  return len;
}

int WsStickyWrap::sendFrame(const char *pdata, uint64_t size, int type) {
  unsigned char frame[10] = {0}; /* Frame.             */
  uint8_t headSize = 0;
  frame[0] = (WS_FIN | type);
  if (size <= 125) {
    frame[1] = size & 0x7F;
    headSize = 2;
  } else if (size >= 126 && size < 65535) {
    frame[1] = 126;
    frame[2] = (size >> 8) & 0xFF;
    frame[3] = size & 0xFF;
    headSize = 4;
  } else {
    frame[1] = 127;
    frame[2] = (unsigned char)((size >> 56) & 0xFF);
    frame[3] = (unsigned char)((size >> 48) & 0xFF);
    frame[4] = (unsigned char)((size >> 40) & 0xFF);
    frame[5] = (unsigned char)((size >> 32) & 0xFF);
    frame[6] = (unsigned char)((size >> 24) & 0xFF);
    frame[7] = (unsigned char)((size >> 16) & 0xFF);
    frame[8] = (unsigned char)((size >> 8) & 0xFF);
    frame[9] = (unsigned char)(size & 0xFF);
    headSize = 10;
  }
  unsigned char *response =
      (unsigned char *)malloc(sizeof(unsigned char) * (headSize + size + 1));
  if (!response) {
    return -1;
  }
  memcpy(response, frame, headSize);
  memcpy(response + headSize, pdata, size);
  response[headSize + size] = '\0';
  return write((char *)response, size + headSize + 1);
}