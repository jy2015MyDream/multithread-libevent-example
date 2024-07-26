
#ifndef __WSCONNECT_H_
#define __WSCONNECT_H_

#include "../common/BaseType.h"
#include "WsStickyWrap.h"
#include "event2/event.h"
#include "../tcp/TcpStickyWrap.h"
class WsConnectMgr;

class WsConnect : public WsStickyWrap
{
public:
    WsConnect(int fd, UQType connId,WsConnectMgr * pConnectMgr);
    virtual ~WsConnect();
    virtual int readData();
    int do_handshake(char * p);
    virtual int notifyred() { return readData(); }
    virtual void MakeServerMsg(UChar *buff, PKLType msgId , PKLType len);
    UQType GetConnonId();
    WsConnectMgr * GetWsConnectMgr();
    void setEv(event* ev);
private:
    bool _Ishandshark;
    UQType _connonId;
    WsConnectMgr *_pConnectMgr;
    event * _ev;
};
#endif