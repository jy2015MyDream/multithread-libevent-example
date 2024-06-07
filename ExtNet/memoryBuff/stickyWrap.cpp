
#include "stickyWrap.h"

#include <assert.h>

#include "memoryBuff.h"
StickyWrap::StickyWrap(FdBridge *pFb) {
  _pData = (unsigned char *)malloc(4096);
  _Data_size = 0;
  _Data_cap = 4096;
  _head._msgId = 0;
  _head._len = 0;
  _pFb = pFb;
  headSize = sizeof(msgHead);
}

StickyWrap::~StickyWrap() {}

// int StickyWrap::read(int fd)
// {
//     int n = 0;
//     while (true)
//     {
//         unsigned short len = 0;
//         unsigned char *buff = nullptr;
//         _buff.GetWriteBuff(buff, len);
//         n = recv(fd, (char *)buff, len, 0);
//         if (n == 0)
//         {
//             return -1;
//         }
//         if (n < 0)
//         {
//             if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
//             {
//                 break;
//             }
//             else
//             {
//                 return -1; // error
//             }
//         }
//         unsigned short write_size = n;
//         _buff.AdjustWriteSize(write_size);
//         if (n < len)
//         {
//             break;
//         }
//     }
//     return 0;
// }

// void StickyWrap::Addbuff(char *buff, unsigned int len) {
//   if (_Data_size + len < _Data_cap) {
//     memcpy(_pData + _Data_size, buff, len);
//     _Data_size += len;
//     checkFinishPack();
//   }
// }
// bool StickyWrap::checkFinishPack() {
//   if (_head._msgId == 0) {
//     if (_Data_size > sizeof(msgHead)) {
//         memcpy(&_head,_pData,sizeof(_head));
//     }
//   }
//   else if (_head._len < _Data_size){
//     msgData msg;
//     memcpy(&msg._head,_pData,sizeof(msg._head));
//     memcpy(msg._pData,_pData+sizeof(msg._head),msg._head._len -
//     sizeof(msg._head)); FinishPack(&msg);
//   }
// }
void StickyWrap::readData() { 
    memoryBuff *pbuff = _pFb->GetBuffPtr();
    unsigned short len = 0;
    assert(_Data_cap >= _Data_size);
    // 这里肯定有问题 必定会G 想个办法 解决内存不连续的问题
    pbuff->GetReadBuff(_pData+_Data_size,len);
    if(len > 0 )
    {
        if(headSize > _Data_size + len)
        {
            return;
        }
        memcpy((void *)&_head,_pData,headSize);
        if(_head._len >= _Data_size + len)
        {
            // 已经够一个包体
        }

    }

}