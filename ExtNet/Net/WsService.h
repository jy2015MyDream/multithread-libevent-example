

#ifndef __NET_WSSERVICE__H
#define __NET_WSSERVICE__H
#include <string>
#include <thread>
#include "../common/Singleton.h"
#include "../ws/WsConnectMgr.h"
#include "event2/event.h"
#include <mutex>
class WsService : public Singleton<WsService> {
 public:
  WsService();
  ~WsService();
  bool OnStart();
  void join();

  event_base *GetEventBase();
  void doAccept(int fd);
  void SetAddrAndPort(std::string addr, int port);
  void OnStop();
  void AddPack(ServerPack *msg);
  void Swap(std::list<ServerPack*> &list);
 private:
  int openLister();
  int dispatcher();

 public:
  void loop();

 private:
  struct event_base *_ev_base;
  std::thread *_handle;
  int _port;
  std::string _addr;
  bool state;
  event *_ev;
  WsConnectMgr _wsconnectMgr;
  int _listenfd;
  std::list<ServerPack*> packlist;
  
  std::mutex _mutex;
};
#endif