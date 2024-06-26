

#include <errno.h>
#include <error.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif
#include <string>

#ifndef __WSSTICKYWRAP__H_
#define __WSSTICKYWRAP__H_
#include "../common/BaseType.h"
#include "../common/FdBridge.h"
#include "../common/ws.h"
#include "../memoryBuff/memoryBuff.h"

class WsStickyWrap : public WsDealWith, public FdBridge {
 public:
  WsStickyWrap(int fd);
  ~WsStickyWrap();
  virtual int readData(char *pbuff, uint16_t len);
  virtual void MakeServerMsg(UChar *buff, PKLType len) = 0;
  virtual int parseWsHead(char *p, int len);
  virtual int finishParse(char *pData, int size);
  unsigned short CalFrameHeadSize();
  int sendFrame(const char *pdata, uint64_t size, int type);
};
#endif