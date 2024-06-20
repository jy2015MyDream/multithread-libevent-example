
#ifndef __WSCONNECT_H_
#define __WSCONNECT_H_

#include "../common/BaseType.h"
#include "WsStickyWrap.h"
class connectMgr;

class WsConnect : public WsStickyWrap
{
public:
    WsConnect(int fd, UQType connId,connectMgr * pConnectMgr);
    virtual int readData();
    int do_handshake(char * p);
    virtual int notifyred() { return readData(); }
private:
    bool _Ishandshark;

};
#endif