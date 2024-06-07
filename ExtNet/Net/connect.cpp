
#include "connect.h"

connect::connect(int fd, readCB p)
{
}
connect::~connect()
{
}
int connect::read()
{
    _buff.read(_fd);
}