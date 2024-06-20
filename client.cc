
#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ExtNet/common/BaseType.h"

void read_cb(struct bufferevent *bev, void *arg) {
  char buff[1024] = {0};
  bufferevent_read(bev, buff, sizeof(buff));
  printf("fwq say :%s\n", buff);
  // bufferevent_write(bev,buff,sizeof(buff));
  // sleep(100);
}

void write_cb(struct bufferevent *bev, void *arg) {
  char args[1024] = {0};
  printf("=====>>>>\n", bufferevent_read(bev, args, 1024));
  printf("===================我是客户端什么都没做\n");
}

void event_cb(struct bufferevent *bev, short events, void *arg) {
  printf("event_cb=%d", events);
  if (events & BEV_EVENT_EOF) {
    printf("connect close\n");
  } else if (events & BEV_EVENT_ERROR) {
    printf("SOME OTHER ERROR\n");

  } else if (events & BEV_EVENT_CONNECTED) {
    printf("aready connect server\n");
    return;
    /* code */
  }

  bufferevent_free(bev);
}

void read_terminal(evutil_socket_t fd, short what, void *agrs) {
  char buff[1024] = {0};

  int len = read(fd, (void *)buff, sizeof(buff));
  printf("terminal input =%s\n", buff);
  struct bufferevent *bev = (struct bufferevent *)agrs;
  char sendbuff[1024] = {0};
  msgHead *head = (msgHead *)sendbuff;
  head->_len = sizeof(msgHead) + len;
  head->_msgId = 100;
  memcpy(sendbuff + sizeof(msgHead), buff, len);
  bufferevent_write(bev, sendbuff, len + sizeof(msgHead));
}

int main() {
  struct event_base *base = NULL;
  base = event_base_new();
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct bufferevent *bev = nullptr;
  bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
  struct sockaddr_in serv;
  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(10086);
  inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);

  bufferevent_socket_connect(bev, (struct sockaddr *)&serv, sizeof(serv));
  bufferevent_setcb(bev, read_cb, write_cb, event_cb, nullptr);
  struct event *ev =
      event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, read_terminal, bev);
  bufferevent_enable(bev, EV_READ | EV_WRITE);
  event_add(ev, NULL);
  event_base_dispatch(base);
  event_free(ev);
  event_base_free(base);
  return 0;
}