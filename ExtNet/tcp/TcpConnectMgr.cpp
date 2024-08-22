
#include "TcpConnectMgr.h"

#include "TcpConnect.h"
#include "../Net/TcpService.h"

TcpConnectMgr::TcpConnectMgr(TcpService *pservice) : _pService(pservice) {}

// void TcpConnectMgr::PushMsg(ServerPack *pack) { _packList.push_back(pack); }

TcpConnect *TcpConnectMgr::NewConnect(int fd) {
  TcpConnect *pConn = new TcpConnect(fd, Make_uniqueId(), this);
  _connectMgr.insert(std::make_pair(pConn->GetConnonId(), pConn));
  return pConn;
}

TcpConnectMgr::~TcpConnectMgr() {
  OnStop();
  _pService = nullptr;
  _UniqueId = 0;
}
TcpService *TcpConnectMgr::GetTcpService() { return _pService; }
UQType TcpConnectMgr::Make_uniqueId() { return _UniqueId++; }
void TcpConnectMgr::deleteConnect(UQType connId) { _connectMgr.erase(connId); }
void TcpConnectMgr::OnStop() {
  for (auto &it : _connectMgr) {
    delete it.second;
    it.second = nullptr;
  }
  _connectMgr.clear();
  // if (!_packList.empty()) {
  //   // 包还没处理完 直接删除 以后再说
  //   for (auto pack : _packList) {
  //     delete pack;
  //     pack = nullptr;
  //   }
  // }
  // _packList.clear();
}