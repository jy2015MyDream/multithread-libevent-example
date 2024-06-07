

#include "FdBridge.h"
#include "memoryBuff/stickyWrap.h"

class TcpConnect : public StickyWrap, FdBridge {
 public:
  TcpConnect(int fd) : FdBridge(fd), StickyWrap(this) {}

 public:
  virtual void notifyred() { readData(); }
};