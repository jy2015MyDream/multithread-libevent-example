
#include <errno.h>
#include <error.h>

#include <functional>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#include "../memoryBuff/memoryBuff.h"
typedef std::function<void(const char *pbuff, unsigned int len)> readCB;
typedef std::function<void()> notifyCB;
class FdBridge {
 private:
  int _fd;
  readCB _callback;
  notifyCB _notify_read;
  memoryBuff _buff;

 public:
  FdBridge() = delete;
  FdBridge(int fd) {
    _fd = fd;
  }
  void setReadNotify(notifyCB notify){
    _notify_read = notify;
  }
  ~FdBridge();
  int read() {
    int n = 0;
    while (true) {
      unsigned short len = 0;
      unsigned char *buff = nullptr;
      _buff.GetWriteBuff(buff, len);
      n = recv(_fd, (char *)buff, len, 0);
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
    notifyred();
    return 0;
  }
  virtual void notifyred() = 0;
  void write();
  bool checkFinishPack();
  memoryBuff * GetBuffPtr(){
    return &_buff;
  }
};