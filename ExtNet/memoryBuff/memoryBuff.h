
/*
    auth: Dragon
    last modify: 2024.5.27

    memory buff，this buff use list
    this buff must be new() construction,
*/


#pragma once
#ifndef __MEMORY_BUFF_H_
#define __MEMORY_BUFF_H_
#define _DEFAULT_MEMORY_LIST_NODE_ 2
#define _DEFAULT_NODE_FREE_COUNT 5
#include "ListEx.h"

class memoryBuff
{
private:
    ListNode *_pTail;
    ListNode *_pRead;
    ListNode *_pWrite;
    unsigned short _freeCount;
public:
    memoryBuff(unsigned char len = _DEFAULT_MEMORY_LIST_NODE_);
    ~memoryBuff();
    memoryBuff(memoryBuff &mem) = delete;
    memoryBuff &operator=(const memoryBuff &) = delete;
    /*
    get write buff addr and buff size, input data to this buff,but input data size lt len
    */
    unsigned char * GetWriteBuff(unsigned short &len);
    /*
        real write date size, otherwise assert fail
    */
    void AdjustWriteSize(unsigned short &len);
    /*
    get read buff point and buff size,out put data size lt len
    */
    unsigned char * GetReadBuff(unsigned short &len);
    /*
        real read data size，otherwise assert fail
    */
    void AdjustReadSize(unsigned short len);
    char *GetChar();
};
#endif
