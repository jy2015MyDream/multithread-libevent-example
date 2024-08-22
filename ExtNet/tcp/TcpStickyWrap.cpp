
#include "TcpStickyWrap.h"

#include <assert.h>
#include <string.h>

#include <cstring>

#include "../common/FdBridge.h"
#include "../common/sha1.h"
#include "../common/ws.h"
#include "../memoryBuff/memoryBuff.h"

TcpStickyWrap::TcpStickyWrap(int fd) : FdBridge(fd) {}

TcpStickyWrap::~TcpStickyWrap() {}

int TcpStickyWrap::readData() {
  memoryBuff *pbuff = GetBuffPtr();
  int realLen = 0;
  while (true) {
    unsigned short len = 0;
    unsigned short readlen = 0;
    unsigned char *pReadBuff = pbuff->GetReadBuff(len);

    if (_Data_size < _headSize) {
      size_t cpsize =
          len > _headSize - _Data_size ? _headSize - _Data_size : len;
      unsigned char *pWrite = _pData + _Data_size;
      memcpy(pWrite, pReadBuff, cpsize);
      readlen += len > _headSize - _Data_size ? _headSize - _Data_size : len;
      _Data_size += len > _headSize - _Data_size ? _headSize - _Data_size : len;
    }
    if (_Data_size >= _headSize &&
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
      readlen += _head->_len - _Data_size;
      _Data_size += _head->_len - _Data_size;
    }
    pbuff->AdjustReadSize(readlen);
    realLen += readlen;
    if (_Data_size > _headSize && _head->_len == _Data_size) {
      // 一个包体完成
      MakeServerMsg(_pData, _head->_msgId,_head->_len);
      _Data_size = 0;
    }

    if (len <= readlen) {
      break;
    }
  }
  return realLen;
}