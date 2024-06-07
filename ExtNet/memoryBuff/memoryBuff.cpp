

#include "memoryBuff.h"
#include <stdio.h>
#include <assert.h>

memoryBuff::memoryBuff(unsigned char len)
{
    _freeCount = len-1;
    _pTail = nullptr;
    for (int i = 0; i < len; ++i)
    {
        if (_pTail == nullptr)
        {
            _pWrite = _pRead = _pTail = new ListNode();
            _pTail->_next = nullptr;
        }
        else
        {
            ListNode *pNode = new ListNode();
            _pTail->_next = pNode;
            pNode->_next = nullptr;
        }
    }
}
memoryBuff::~memoryBuff()
{
    while (_pRead)
    {
        ListNode *pNode = _pRead->_next;
        delete _pRead;
        _pRead = pNode;
    }
    _pWrite = nullptr;
    _pTail = nullptr;
}

void memoryBuff::GetWriteBuff(unsigned char *buff, unsigned short &len)
{
    if (_pWrite->_cap - _pWrite->_write_size > 0)
    {
        buff = _pWrite->_pData + _pWrite->_write_size;
        len = _pWrite->_cap - _pWrite->_write_size;
    }
    else
    {
        _freeCount += 1; 
        ListNode *pNode = new ListNode();
        _pTail->_next = pNode;
        _pTail = pNode;
        _pWrite = pNode;
        buff = _pWrite->_pData;
        len = _pWrite->_cap;
    }
}
void memoryBuff::GetReadBuff(unsigned char *buff, unsigned short &len)
{
    if (_pRead->_write_size > _pRead->_read_size)
    {
        buff = _pRead->_pData + _pRead->_read_size;
        len = _pRead->_write_size - _pRead->_read_size;
    }
}

void memoryBuff::AdjustWriteSize(unsigned short &len)
{

#ifndef _PRINTF_LOG
    assert(_pWrite->_cap - _pWrite->_write_size >= len);
    _pWrite->_write_size += len;
#else
    if (_pWrite->_cap - _pWrite->_write_size < len)
    {
        printf("memory Buff AdjustWriteSize error, len gt last cap,last cap=%d,len=%d\n", _pWrite->_cap - _pWrite->_write_size, len);
        assert(false);
    }
    _pWrite->_write_size += len;
#endif
    if(_pWrite->_write_size >= _pWrite->_cap)
    {
        _freeCount -= 1;
        _pWrite = _pWrite->_next;
    }
}

void memoryBuff::AdjustReadSize(unsigned short &len)
{
    if (_pRead == _pWrite)
    {
#ifndef _PRINTF_LOG
        assert(_pRead->_write_size - _pRead->_read_size >= len);
        _pRead->_read_size += len;
#else
        if (_pRead->_write_size - _pRead->_read_size < len)
        {
            printf("memory Buff AdjustReadSize error, len lt last cap,last cap=%d,len=%d\n", _pRead->_write_size - _pRead->_read_size, len);
            assert(false);
        }
        _pRead->_read_size += len;
#endif
    }
    else
    {
        if (_pRead->_cap - _pRead->_read_size < len)
        {
            printf("memory Buff AdjustReadSize error, len lt last cap,last cap=%d,len=%d\n", _pRead->_write_size - _pRead->_read_size, len);
            assert(false);
        }
        _pRead->_read_size += len;
    }
    if (_pRead->_cap <= _pRead->_read_size)
    {
        ListNode *tmpNode = _pRead;
        _pRead = _pRead->_next;
        tmpNode->ResetNode();
        if (_freeCount > _DEFAULT_NODE_FREE_COUNT)
        {
            delete tmpNode;
        }
        else
        {
            _freeCount += 1;
            _pTail->_next = tmpNode;
            _pTail = tmpNode;
        }
    }
}
