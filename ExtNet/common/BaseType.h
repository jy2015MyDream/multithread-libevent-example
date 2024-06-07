
#ifndef _BASE_TYPE_H_
#define _BASE_TYPE_H_

typedef struct msgHead
{
    unsigned short _len;
    unsigned short _msgId;
    msgHead(){
        _len =0;
        _msgId = 0;
    }
};

typedef struct msgData
{
    msgHead _head;
    unsigned char *_pData;
};
#endif