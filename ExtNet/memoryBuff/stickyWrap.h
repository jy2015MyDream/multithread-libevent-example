

#include <errno.h>
#include <error.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif
#include <string>

#ifndef __STICKYWRAP__H_
#define __STICKYWRAP__H_
#include "../common/BaseType.h"
#include "memoryBuff.h"
#include "../common/ws.h"
class FdBridge;
class StickyWrap {
 private:
  msgHead *_head;
  unsigned char *_pData;
  unsigned short _Data_size;
  unsigned short _Data_cap;
  FdBridge *_pFb;
  unsigned int headSize;
  bool _Ishandshake;
  ws_head _wshead;

 public:
  StickyWrap();
  ~StickyWrap();
  virtual int readData();
  virtual void setFdBridge(FdBridge *pFb);
  virtual void MakeServerMsg(UChar *buff, PKLType len) = 0;
  virtual int do_handshake(char * p);
  virtual int parseWsHead(char *p,int len);
};
#endif