

#ifndef __TCPCONNECT__H__
#define __TCPCONNECT__H__ 3
#include "event2/event.h"
#include "../common/BaseType.h"
#include "TcpStickyWrap.h"
class TcpConnectMgr;

class TcpConnect : public TcpStickyWrap {
 public:
  TcpConnect(int fd, UQType connId,TcpConnectMgr * pConnectMgr);
  TcpConnect() = delete;
  TcpConnect(const TcpConnect &connn) = delete;
  TcpConnect &operator=(const TcpConnect &) = delete;
  void PushPack(ServerPack *pack);
  UQType GetConnonId();
  TcpConnectMgr *GetTcpConnectMgr();
  void setEv(event *ev);
  virtual ~TcpConnect();
  event *GetEvent();

 private:
  UQType _connonId;
  TcpConnectMgr *_connMgr;
  event *_ev;

 public:
  virtual int notifyred() { return readData(); }
  virtual void MakeServerMsg(UChar *buff,PKLType msgId, PKLType len);
};
#endif