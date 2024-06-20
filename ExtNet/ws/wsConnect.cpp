
#include "wsConnect.h"

wsConnect::wsConnect(int fd, UQType connId,connectMgr * pConnectMgr):TcpConnect(fd,connId,pConnectMgr){
    _Ishandshark = false;
}

int wsConnect::readData(){
  memoryBuff *pbuff = _pFb->GetBuffPtr();
  int realLen = 0;
  while (true) {
    unsigned short len = 0;
    unsigned short readlen = 0;
    unsigned char *pReadBuff = pbuff->GetReadBuff( len);
    if (len > 0) {
      if(!_Ishandshark)
      {
        char buff[1024]={0};
        memcpy(buff,pReadBuff,len);
        printf("%s",buff);
      }
      break;
    }
    
  }
  return realLen;
}