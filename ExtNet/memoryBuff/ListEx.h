
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __LIST_EX_H_
#define __LIST_EX_H_
#define _LIST_NODE_DATA_SIZE 4096
class ListNode
{
public:
    unsigned char *_pData;
    unsigned short _cap;
    unsigned short _write_size;
    unsigned short _read_size;

public:
    ListNode *_next;

public:
    ListNode(/* args */)
    {
        ResetNode();
    }
    void ResetNode()
    {
        _pData = (unsigned char *)malloc(_LIST_NODE_DATA_SIZE);
        memset(_pData, 0, _LIST_NODE_DATA_SIZE);
        _cap = _LIST_NODE_DATA_SIZE;
        _write_size = 0;
        _read_size = 0;
        _next = nullptr;
    }
    ~ListNode()
    {
        free(_pData);
        _cap = 0;
        _write_size = 0;
        _read_size = 0;
        _next = nullptr;
    }
};
#endif