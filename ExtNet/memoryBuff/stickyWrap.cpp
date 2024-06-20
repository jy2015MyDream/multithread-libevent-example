
#include "stickyWrap.h"

#include <assert.h>
#include <string.h>

#include <cstring>

#include "../common/FdBridge.h"
#include "../common/sha1.h"
#include "../common/ws.h"
#include "memoryBuff.h"

StickyWrap::StickyWrap() {
  _pData = (unsigned char *)malloc(4096);
  memset(_pData, 0, 4096);
  _Data_size = 0;
  _Data_cap = 4096;
  _head = (msgHead *)_pData;
  headSize = sizeof(msgHead);
  _Ishandshake = false;
}

StickyWrap::~StickyWrap() {
  free(_pData);
  _head = nullptr;
  _pData = nullptr;
  _Data_size = 0;
  _Data_cap = 0;
  _pFb = nullptr;
  headSize = 0;
}

void StickyWrap::setFdBridge(FdBridge *pFb) { _pFb = pFb; }

int StickyWrap::do_handshake(char *buff) {
  const char *p = strstr(buff, "\r\n\r\n");
  if (p == nullptr) {
    return -1;
  }
  char *response; /* Handshake response message. */
  /* Get response. */
  if (get_handshake_response(buff, &response) < 0) {
    DEBUG("Cannot get handshake response, request was: %s\n", wfd->frm);
    return (-1);
  }
  if (send(_pFb->getFd(), response, strlen(response), MSG_NOSIGNAL) < 0) {
    free(response);
    DEBUG("As error has occurred while handshaking!\n");
    return (-1);
  }
  return 0;
}
int StickyWrap::parseWsHead(char *pReadBuff, int len) {
  int readlen = 0;
  if (len > 0) {
    if (len > sizeof(ws_base)) {
      _wshead.len = 0;
      //_wshead.masks = {0};
      memset(_wshead.masks, 0, sizeof(_wshead.masks));
      //_wshead.wbs = (ws_base *)pReadBuff;
      _wshead.wbs = (ws_base *)malloc(2);
      memset(_wshead.wbs, 0, 2);
      memcpy(_wshead.wbs, pReadBuff, 2);
      readlen += sizeof(ws_base);
      pReadBuff += sizeof(ws_base);
      if (_wshead.wbs->payLoad < 126) {
        _wshead.len = _wshead.wbs->payLoad;
      } else if (_wshead.wbs->payLoad == 126) {
        _wshead.len = (((uint64_t)pReadBuff[0]) << 8) | pReadBuff[1];
        readlen += 2;
        pReadBuff += 2;
      } else {
        _wshead.len = (((uint64_t)pReadBuff[0]) << 56) |
                      (((uint64_t)pReadBuff[1]) << 48) |
                      (((uint64_t)pReadBuff[2]) << 40) |
                      (((uint64_t)pReadBuff[3]) << 32) |
                      (((uint64_t)pReadBuff[4]) << 24) |
                      (((uint64_t)pReadBuff[5]) << 16) |
                      (((uint64_t)pReadBuff[6]) << 8) |
                      (((uint64_t)pReadBuff[7]));
        readlen += 8;
        pReadBuff += 8;
      }
      if (_wshead.len > 65536) {
        return -1;
      }
      _wshead.masks[0] = pReadBuff[0];
      _wshead.masks[1] = pReadBuff[1];
      _wshead.masks[2] = pReadBuff[2];
      _wshead.masks[3] = pReadBuff[3];
      readlen += 4;
      pReadBuff += 4;
      for (int i = 0; i < _wshead.len; i++) {
        char cur_byte = pReadBuff[i];
        if (cur_byte == -1) return (-1);
        pReadBuff[i] = cur_byte ^ _wshead.masks[i % 4];
      }
    }
  }
  return readlen;
}
int StickyWrap::readData() {
  memoryBuff *pbuff = _pFb->GetBuffPtr();
  int realLen = 0;
  while (true) {
    unsigned short len = 0;
    unsigned short readlen = 0;
    unsigned char *pReadBuff = pbuff->GetReadBuff(len);
    // pbuff->AdjustReadSize()
    if (!_Ishandshake) {
      do_handshake((char *)pReadBuff);
      pbuff->AdjustReadSize(len);
      _Ishandshake = true;
      realLen += len;
    } else {
      // 解包
      int readsize = parseWsHead((char *)pReadBuff, len);
      if (readsize < 0) {
        return readsize;
      }
      pbuff->AdjustReadSize((unsigned short)readsize);
      len = len - readsize;
      pReadBuff += readsize;
      printf("%s\n", pReadBuff);
      if (len > 0) {
        if (_Data_size < headSize) {
          size_t cpsize =
              len > headSize - _Data_size ? headSize - _Data_size : len;
          unsigned char *pWrite = _pData + _Data_size;
          memcpy(pWrite, pReadBuff, cpsize);
          readlen += len > headSize - _Data_size ? headSize - _Data_size : len;
          _Data_size +=
              len > headSize - _Data_size ? headSize - _Data_size : len;
        }
        if (_Data_size >= headSize &&
            _head->_len >
                _Data_cap)  // 数据超过一个包的数据，认为是非法数据 需要close
        {
          return -1;
        }
        if (_head->_len >= _Data_size + (len - readlen)) {
          memcpy(_pData + _Data_size, pReadBuff, (len - readlen));
          _Data_size += len - readlen;
          readlen = len;
        } else {
          memcpy(_pData + _Data_size, pReadBuff, _head->_len - _Data_size);
          readlen = _head->_len - _Data_size;
          _Data_size += _head->_len - _Data_size;
        }
        pbuff->AdjustReadSize(readlen);
        realLen += readlen;
        if (_Data_size > headSize && _head->_len == _Data_size) {
          // 一个包体完成
          MakeServerMsg(_pData, _head->_len);
          _Data_size = 0;
        }
      }
      if (len <= readlen) {
        break;
      }
    }
    return realLen;
  }
}