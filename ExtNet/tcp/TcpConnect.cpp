#include "TcpConnect.h"

#include "TcpConnectMgr.h"

TcpConnect::TcpConnect(int fd, UQType connId, TcpConnectMgr* pConnectMgr)
    : TcpStickyWrap(fd),
      _connonId(connId),
      _connMgr(pConnectMgr),
      _ev(nullptr) {
}

void TcpConnect::MakeServerMsg(UChar* buff, PKLType len) {
  ServerPack* pack = new ServerPack(_connonId, buff, len);
  _connMgr->PushMsg(pack);
}
UQType TcpConnect::GetConnonId() { return _connonId; }

TcpConnectMgr* TcpConnect::GetTcpConnectMgr() { return _connMgr; }

void TcpConnect::setEv(event* ev) { _ev = ev; }
TcpConnect::~TcpConnect() {
  _connonId = 0;
  _connMgr = nullptr;
  event_free(_ev);
  _ev = nullptr;
}
event* TcpConnect::GetEvent() { return _ev; }