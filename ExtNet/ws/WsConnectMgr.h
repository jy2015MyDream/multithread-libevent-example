

#ifndef __WSCONNECT_MGR_H_
#define __WSCONNECT_MGR_H_
#include "../common/BaseType.h"
#include <list>
#include <unordered_map>
class WsConnect;
class WsService;
class WsConnectMgr {
 private:
  std::unordered_map<UQType, WsConnect *> _wsconnectMgr;
  std::list<ServerPack *> _packList;
  UQType _UniqueId;
  WsService *_pService;

 public:
  WsConnectMgr(WsService *pservice);
  void PushMsg(ServerPack *pack);
  WsConnect *NewConnect(int fd);
  ~WsConnectMgr();
  WsService *GetWsConnectMgr();
  UQType Make_uniqueId();
  void deleteConnect(UQType connId);
  void OnStop();
};
#endif