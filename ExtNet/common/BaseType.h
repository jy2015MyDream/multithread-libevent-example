
#include <stdint.h>

#include <cstdlib>
#include <cstring>
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
  PackDealWith() {
    _headSize = sizeof(msgHead);
    _pData = (uint8_t *)malloc(PACK_CAP);
    _Data_size = 0;
    _Data_cap = PACK_CAP;
  }
  ~PackDealWith() {
    free(_pData);
    _pData = nullptr;
    _Data_size = 0;
    _Data_cap = 0;
  }
};
typedef struct Ws_Base {
  unsigned char opcod : 4;
  unsigned char Rsv : 3;
  unsigned char frist : 1;
  unsigned char payLoad : 7;
  unsigned char mask : 1;
} ws_base;

typedef struct WS_HEAD {
  ws_base *wbs;
  unsigned long int len;
  uint8_t masks[4];
} ws_head;

enum OP_State{
    INIT_HEAD=1,
    READ_LEN=2,
    READ_MASK=3,
    READ_MASK_KEY=4,
    READ_DATA=5,
};

struct WsDealWith {
  ws_head _head;
  unsigned char *_pData;
  unsigned short _Data_size;
  unsigned short _Data_cap;
  unsigned short _Read_size;
  unsigned short _parse_Index;
  uint16_t _headSize;
  uint8_t _Op;
  WsDealWith() {
    _headSize = sizeof(ws_base);
    _pData = (uint8_t *)malloc(PACK_CAP*2);
    _Data_size = 0;
    _Data_cap = PACK_CAP*2;
    _Op = 1;
    _Read_size = 0;
    _parse_Index = 0;
  }
  ~WsDealWith() {
    free(_pData);
    _pData = nullptr;
    _Data_size = 0;
    _Data_cap = 0;
  }
};
#endif