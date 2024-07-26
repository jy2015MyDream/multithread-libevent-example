

#include "WsService.h"

#include <arpa/inet.h>
#include <event2/thread.h>
#include <unistd.h>

#include "../ws/WsConnect.h"
#include "../ws/WsConnectMgr.h"

void ws_client_read_cb(evutil_socket_t client, short what, void *ptr) {
  WsConnect *pConn = (WsConnect *)ptr;
  int ret = 0;
  if (what & EV_READ) {
    ret = pConn->read();
    if (ret < 0) {
      pConn->GetWsConnectMgr()->deleteConnect(pConn->GetConnonId());
      WsConnectMgr *wsconnect = pConn->GetWsConnectMgr();
      wsconnect->deleteConnect(pConn->GetConnonId());
      delete pConn;
      return;
    }
  }
}

void ws_listen_cb(evutil_socket_t fd, short what, void *ptr) {
  WsService *netservicer = (WsService *)ptr;
  if (what | EV_READ) {
    netservicer->doAccept(fd);
  }
}
void WsService::SetAddrAndPort(std::string addr, int port) {
  _addr = addr;
  _port = port;
}
WsService::WsService() : state(false), _wsconnectMgr(this) {
}
WsService::~WsService() {
  delete _handle;
  _handle = nullptr;
  event_base_free(_ev_base);
  _ev_base = nullptr;
}

void WsService::doAccept(int fd) {
  sockaddr_in sa;
  socklen_t len = sizeof(sa);
  int clientfd = accept(fd, (struct sockaddr *)&sa, &len);
  if (clientfd == -1) {
    printf("accept error");
  }
  if (evutil_make_socket_nonblocking(clientfd) < 0) {
    close(clientfd);
    return;
  }
  WsConnect *pConn = _wsconnectMgr.NewConnect(clientfd);
  event *ev = event_new(_ev_base, clientfd, EV_READ | EV_PERSIST,
                        ws_client_read_cb, pConn);
  event_add(ev, nullptr);
  pConn->setEv(ev);
}

int WsService::openLister() {
  evthread_use_pthreads();
  _ev_base = event_base_new();
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  _listenfd = listenfd;
  if (listenfd < 0) {
    printf("socket() failed,error=%d,%s", errno, strerror(errno));
    return listenfd;
  }

  evutil_make_listen_socket_reuseable(listenfd);
  evutil_make_listen_socket_reuseable_port(listenfd);
  if (evutil_make_socket_nonblocking(listenfd) < 0) {
    printf("evutil_make_socket_nonblocking error=%d,%s", errno,
           strerror(errno));
    return -1;
  }
  struct sockaddr_in sockaddr;
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = inet_addr(_addr.c_str());
  sockaddr.sin_port = htons((unsigned short)_port);
  int ret = bind(listenfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
  if (ret < 0) {
    printf("bind() error=%d,%s", errno, strerror(errno));
    return -1;
  }
  ret = listen(listenfd, 256);
  if (ret < 0) {
    printf("listen() error=%d,%s", errno, strerror(errno));
    return -1;
  }
  _ev = event_new(_ev_base, listenfd, EV_READ | EV_PERSIST, ws_listen_cb, this);
  event_add(_ev, nullptr);
  return 0;
}

bool WsService::OnStart() {
  if (openLister() == 0) {
    printf("listen suceess\n");
  } else {
    return false;
  }
  _handle = new std::thread([this]() { this->loop(); });
  return true;
}

void WsService::join() { _handle->join(); }

event_base *WsService::GetEventBase() { return _ev_base; }
int WsService::dispatcher() {
  if (_ev_base) return event_base_loop(_ev_base, EVLOOP_NO_EXIT_ON_EMPTY);
  return -1;
}

void WsService::loop() { dispatcher(); }
void WsService::OnStop() {
  event_free(_ev);
  _ev = nullptr;
  _wsconnectMgr.OnStop();
  close(_listenfd);
  timeval tv;
  tv.tv_sec = 3;
  int r = event_base_loopexit(_ev_base, nullptr);
  join();
    if (!packlist.empty()) {
    // 包还没处理完 直接删除 以后再说
    for (auto pack : packlist) {
      delete pack;
      pack = nullptr;
    }
  }
  packlist.clear();
}

void WsService::AddPack(ServerPack *msg){
  std::lock_guard<std::mutex> lock(_mutex);
  packlist.push_back(msg);
}

void WsService::Swap(std::list<ServerPack*> &list){
  std::lock_guard<std::mutex> lock(_mutex);
  packlist.swap(list);
}