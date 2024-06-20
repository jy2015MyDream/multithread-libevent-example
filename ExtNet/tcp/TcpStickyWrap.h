

#include <errno.h>
#include <error.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif
#include <string>

#ifndef __TCPSTICKYWRAP__H_
#define __TCPSTICKYWRAP__H_
#include "../common/BaseType.h"
#include "../memoryBuff/memoryBuff.h"
#include "../common/ws.h"
#include "../common/FdBridge.h"

class TcpStickyWrap : public PackDealWith , public FdBridge {
 public:
  TcpStickyWrap(int fd);
  ~TcpStickyWrap();
  virtual int readData();
  virtual void MakeServerMsg(UChar *buff, PKLType len) = 0;
};
#endif