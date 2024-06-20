
#include "FdBridge.h"
#include <unistd.h>
FdBridge::FdBridge(int fd) { _fd = fd; }
void FdBridge::setReadNotify(notifyCB notify) { _notify_read = notify; }
FdBridge::~FdBridge(){
  close(_fd);
  _fd = 0;
}
int FdBridge::getFd(){
  return _fd;
}

int FdBridge::read() {
  int n = 0;
  while (true) {
    unsigned short len = 0;
    unsigned char *buff = _buff.GetWriteBuff(len);
    if(buff == nullptr || len == 0)
    {
      printf("not buff\n");
      break;
    }
    n = recv(_fd, (void *)buff, len, 0);
    if (n == 0) {
      return -1;
    }
    if (n < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
        break;
      } else {
        return -1;  // error
      }
    }
    unsigned short write_size = n;
    _buff.AdjustWriteSize(write_size);
    if (n < len) {
      break;
    }
  }
  return notifyred();
}

void FdBridge::write(){
    
}
bool FdBridge::checkFinishPack(){
    return false;
}
memoryBuff *FdBridge::GetBuffPtr() { return &_buff; }
