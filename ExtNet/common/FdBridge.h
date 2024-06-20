
#include <errno.h>
#include <error.h>

#include <functional>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#ifndef __FDBRIDGE_H__
#define __FDBRIDGE_H__
#include "../memoryBuff/memoryBuff.h"

typedef std::function<void()> notifyCB;
class FdBridge {
 private:
  int _fd;
  notifyCB _notify_read;
  memoryBuff _buff;

 public:
  FdBridge() = delete;
  FdBridge(int fd) ;
  void setReadNotify(notifyCB notify);
  ~FdBridge();
  int getFd();
public:
  int read() ;
  virtual int notifyred() = 0;
  void write();
  bool checkFinishPack();
  memoryBuff * GetBuffPtr();
};
#endif