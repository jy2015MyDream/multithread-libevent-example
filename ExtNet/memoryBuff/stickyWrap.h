

#include "../common/BaseType.h"
#include "memoryBuff.h"
#include <errno.h>
#include <error.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif
#include <string>
#include "../Net/FdBridge.h"
//template<class T>
class StickyWrap
{
private:
    msgHead _head;
    unsigned char * _pData;
    unsigned short _Data_size;
    unsigned short _Data_cap;
    std::string d;
    FdBridge * _pFb;
    unsigned char headSize;
public:
    StickyWrap(FdBridge * pFb);
    ~StickyWrap();
    void readData();
    // void Addbuff(char * buff, unsigned int len);
    // virtual void FinishPack(msgData * pdata) = 0;
    
};
