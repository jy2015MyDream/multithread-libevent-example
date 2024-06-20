
#include "WsConnect.h"

#include "../common/ws.h"

WsConnect::WsConnect(int fd, UQType connId, connectMgr *pConnectMgr)
    : WsStickyWrap(fd) {
  _Ishandshark = false;
}

int WsConnect::readData() {
  memoryBuff *pbuff = GetBuffPtr();
  int realLen = 0;
  unsigned short len = 0;
  unsigned short readlen = 0;
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
      return WsStickyWrap::readData();
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