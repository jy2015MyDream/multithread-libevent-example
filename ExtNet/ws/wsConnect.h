
#ifndef __WSCONNECT_H_
#define __WSCONNECT_H_
#include "../Net/tcpConnect.h"

class wsConnect : public TcpConnect
{
public:
    wsConnect(int fd, UQType connId,connectMgr * pConnectMgr);
    virtual int readData();
private:
    bool _Ishandshark;

};
#endif