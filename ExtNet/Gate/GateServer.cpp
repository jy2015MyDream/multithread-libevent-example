
#include "GateServer.h"
#include "../Net/WsService.h"
#include "../Net/TcpService.h"
int GateServer::OpenServer() {
    WsService::GetIns()->SetAddrAndPort("0.0.0.0",10086);
    if(WsService::GetIns()->OnStart() == false ){
        return -1;
    }
    TcpService::GetIns()->SetAddrAndPort("0.0.0.0",10087);
    if(TcpService::GetIns()->OnStart() == false ){
        return -2;
    }
    _handle = new std::thread([this]() { this->MainThread(); });
}

void GateServer::OnStopServer(){
    
    WsService::GetIns()->OnStop();
    TcpService::GetIns()->OnStop();
    IsRun = false;
    _handle->join();
}


void GateServer::MainThread()
{
    IsRun = true;
    while (IsRun)
    {
        std::list<ServerPack*> list;
        WsService::GetIns()->Swap(list);
        for(auto pack : list){
            
        }
    }
    
}