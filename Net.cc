
#include <unistd.h>
#include <iostream>
#include "ExtNet/Net/TcpService.h"
#include "ExtNet/Net/WsService.h"

int main() {
    // TcpService service;
    // service.SetAddrAndPort("0.0.0.0", 10086);
    // service.OnStart();
    // sleep(3);
    // service.OnStop();

  // TcpService::GetIns()->SetAddrAndPort("0.0.0.0",10086);
  // if(TcpService::GetIns()->OnStart() == false ){
  //   return 0;
  // }
  // while (true)
  // {
  //     char cmdbuf[128] = {0};
  // 	std::cin.getline(cmdbuf, sizeof(cmdbuf));
  //     printf("%s\n",cmdbuf);
  // 	if (strcmp(cmdbuf, "exit") == 0)
  // 		break;
  //     sleep(0.5);
  // }
  // TcpService::GetIns()->OnStop();

  
  WsService::GetIns()->SetAddrAndPort("0.0.0.0",10086);
  if(WsService::GetIns()->OnStart() == false ){
    return 0;
  }
  while (true)
  {
      char cmdbuf[128] = {0};
  	std::cin.getline(cmdbuf, sizeof(cmdbuf));
      printf("%s\n",cmdbuf);
  	if (strcmp(cmdbuf, "exit") == 0)
  		break;
      sleep(0.5);
  }
  WsService::GetIns()->OnStop();


  return 0;
}
