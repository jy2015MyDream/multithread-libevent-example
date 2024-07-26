

#ifndef __NET_SERVICE__H
#define __NET_SERVICE__H
#include <string>
#include <thread>
#include "../common/Singleton.h"
#include "../tcp/TcpConnectMgr.h"
#include "event2/event.h"

class TcpService : public Singleton<TcpService> {
 public:
  TcpService();
  ~TcpService();
  bool OnStart();
  void join();

  event_base *GetEventBase();
  void doAccept(int fd);
  void SetAddrAndPort(std::string addr, int port);
  void OnStop();

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
  TcpConnectMgr _connectMgr;
  int _listenfd;
  std::list<ServerPack *> _packList;
};
#endif