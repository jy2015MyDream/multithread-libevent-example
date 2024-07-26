
#include "WsConnect.h"

#include "../common/ws.h"
#include "WsConnectMgr.h"
WsConnect::WsConnect(int fd, UQType connId, WsConnectMgr *pConnectMgr)
    : WsStickyWrap(fd) {
  _Ishandshark = false;
  _connonId = connId;
  _pConnectMgr = pConnectMgr;
}
WsConnect::~WsConnect() {
  _Ishandshark = false;
  _connonId = 0;
  _pConnectMgr = nullptr;
  event_free(_ev);
  _ev = nullptr;
}

int WsConnect::readData() {
  memoryBuff *pbuff = GetBuffPtr();
  unsigned short readlen = 0;
  while (true) {
    unsigned short len = 0;
    unsigned char *pReadBuff = pbuff->GetReadBuff(len);
    if (len > 0) {
      if (!_Ishandshark) {
        int ret = do_handshake((char *)pReadBuff);
        if (ret < 0) {
          return ret;
        }
        pbuff->AdjustReadSize(len);
        _Ishandshark = true;
        return len;
      } else {
        int rlen = WsStickyWrap::readData((char *)pReadBuff, len);
        if (rlen < 0) return rlen;
        readlen += rlen;
        pbuff->AdjustReadSize(rlen);
      }
    } else {
      break;
    }
  }
  return readlen;
}

int WsConnect::do_handshake(char *buff) {
  const char *p = strstr(buff, "\r\n\r\n");
  if (p == nullptr) {
    return -1;
  }
  char *response;
  if (get_handshake_response(buff, &response) < 0) {
    DEBUG("Cannot get handshake response, request was: %s\n", wfd->frm);
    return (-1);
  }
  if (send(getFd(), response, strlen(response), MSG_NOSIGNAL) < 0) {
    free(response);
    DEBUG("As error has occurred while handshaking!\n");
    return (-1);
  }
  return 0;
}
UQType WsConnect::GetConnonId() { return _connonId; }
WsConnectMgr *WsConnect::GetWsConnectMgr() { return _pConnectMgr; }
void WsConnect::MakeServerMsg(UChar *buff, PKLType msgId , PKLType len) {
  ServerPack* pack = new ServerPack(_connonId, buff, msgId, len);
  //_connMgr->PushMsg(pack);
}
void WsConnect::setEv(event *ev) { _ev = ev; }