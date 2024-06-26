
#include "WsConnectMgr.h"
#include "WsConnect.h"
#include "../Net/WsService.h"

WsConnectMgr::WsConnectMgr(WsService *pservice) : _pService(pservice) {
    _UniqueId = 0;
}

void WsConnectMgr::PushMsg(ServerPack *pack) { _packList.push_back(pack); }

WsConnect *WsConnectMgr::NewConnect(int fd) {
  WsConnect *pConn = new WsConnect(fd, Make_uniqueId(), this);
  _wsconnectMgr.insert(std::make_pair(pConn->GetConnonId(), pConn));
  return pConn;
}

WsConnectMgr::~WsConnectMgr() {
  OnStop();
  _pService = nullptr;
  _UniqueId = 0;
}
WsService *WsConnectMgr::GetWsConnectMgr() { return _pService; }
UQType WsConnectMgr::Make_uniqueId() { return _UniqueId++; }
void WsConnectMgr::deleteConnect(UQType connId) { _wsconnectMgr.erase(connId); }
void WsConnectMgr::OnStop() {
  for (auto &it : _wsconnectMgr) {
    delete it.second;
    it.second = nullptr;
  }
  _wsconnectMgr.clear();
  if (!_packList.empty()) {
    // 包还没处理完 直接删除 以后再说
    for (auto pack : _packList) {
      delete pack;
      pack = nullptr;
    }
  }
  _packList.clear();
}