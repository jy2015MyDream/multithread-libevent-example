

#ifndef __CONNECT_MGR_H_
#define __CONNECT_MGR_H_ 1
#include "../common/BaseType.h"
#include <list>
#include <unordered_map>
class TcpConnect;
class TcpService;
class TcpConnectMgr {
 private:
  std::unordered_map<UQType, TcpConnect *> _connectMgr;
  UQType _UniqueId;
  TcpService *_pService;

 public:
  TcpConnectMgr(TcpService *pservice);
  void PushMsg(ServerPack *pack);
  TcpConnect *NewConnect(int fd);
  ~TcpConnectMgr();
  TcpService *GetTcpService();
  UQType Make_uniqueId();
  void deleteConnect(UQType connId);
  void OnStop();
};
#endif