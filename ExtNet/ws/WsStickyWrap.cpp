
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

int WsStickyWrap::readData() {
  memoryBuff *pbuff = GetBuffPtr();
  int realLen = 0;
  while (true) {
    unsigned short len = 0;
    unsigned char *pReadBuff = pbuff->GetReadBuff(len);
    if (len > 0 && len < _Data_cap - _Data_size) {
      memcpy(_pData + _Data_size, pReadBuff, len);
      _Data_size += len;
      realLen += len;
      if (_Data_size >= _headSize) {
        do {
          int len = parseWsHead((char *)(_pData + _Read_size),
                                _Data_size - _Read_size);
          _Read_size += len;
        } while (len > 0);
      }
    } else {
      printf("long data\n");
    }
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
          _Op = OP_State::READ_LEN;
          pbuff += _headSize;
        }

        break;
      case OP_State::READ_LEN:
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
        if (len - readlen >= 8) {
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
        while (_parse_Index <= _head.len && readlen <= len)
        {
          char cur_byte = *pbuff;
          if (cur_byte == -1) return (-1);
          *pbuff = cur_byte ^ _head.masks[_parse_Index % 4];
        }
        if(_parse_Index > _head.len)
        {
          _Op = OP_State::INIT_HEAD;
        }
      } break;
      default:
        break;
    }
  }
  return readlen;
}