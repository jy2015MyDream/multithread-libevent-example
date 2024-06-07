
#include "../memoryBuff/stickyWrap.h"


#include<functional>

typedef std::function<void(const char * pbuff,unsigned int len)> readCB;
class connect
{
private:
    int _fd;
    unsigned int _connect_Id;
    StickyWrap _buff;
    readCB *_callback;
public:
    connect(/* args */)=delete;
    connect(int fd,readCB p);
    ~connect();
    virtual int read();
    void write();
    bool checkFinishPack();
};