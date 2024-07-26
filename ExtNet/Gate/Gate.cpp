
#include <unistd.h>

#include <iostream>

#include "GateServer.h"
#include <cstring>

int main() {
  GateServer::GetIns()->OpenServer();
  while (true) {
    char cmdbuf[128] = {0};
    std::cin.getline(cmdbuf, sizeof(cmdbuf));
    printf("%s\n", cmdbuf);
    if (std::strcmp(cmdbuf, "exit") == 0) break;
    sleep(0.5);
  }
  GateServer::GetIns()->OnStopServer();
  return 0;
}