
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#ifndef _BASE_TYPE_H_
#define _BASE_TYPE_H_
typedef unsigned int UQType;
typedef unsigned short PKLType;
typedef unsigned char UChar;

typedef struct msgHead {
  PKLType _len;
  PKLType _msgId;
  msgHead() {
    _len = 0;
    _msgId = 0;
  }
} msgHead;
typedef struct ServerPack {
  unsigned char *_pData;
  unsigned short _size;
  ServerPack(UQType connId, UChar *buff, PKLType len) {
    _size = len + sizeof(UQType);
    _pData = (unsigned char *)malloc(_size);
    memset(_pData, 0, _size);
    memcpy(_pData, (void *)&connId, sizeof(UQType));
    memcpy(_pData + sizeof(UQType), buff, len);
  }
  ~ServerPack() { free(_pData); }
} ServerPack;

typedef struct msgData {
  msgHead _head;
  UChar *_pData;
} msgData;

#define PACK_CAP 4096

struct PackDealWith {
  msgHead *_head;
  unsigned char *_pData;
  unsigned short _Data_size;
  unsigned short _Data_cap;
  uint16_t _headSize;
  PackDealWith(){
    _headSize = sizeof(msgHead);
    _pData = (uint8_t *)malloc(PACK_CAP);
    _Data_size = 0;
    _Data_cap = PACK_CAP;
  }
  ~PackDealWith(){
    free(_pData);
    _pData = nullptr;
    _Data_size = 0;
    _Data_cap = 0;
  }
};

#endif