#pragma once




#include <iostream>
using std::cin
using std::cout



template <T>;

class CMP
{



private:

    T* memstart = nullptr;  //内存池的头指针
    T * start = nullptr_t;  //可用内存池的地址

public:
    
    T * new()
    {
            memstart=(T *)malloc(sizeof(T));
            start = memstart;
    }

};
     
template<class T>
class ObjectPool
{
public:
    T* New()
    {
        T* obj = nullptr;
        // 
        如果?由链表有对象，直接取?个

            if (_freeList)
            {
                obj = (T*)_freeList;
                _freeList = *((void**)_freeList);
            }
            else
            {
                if (_leftBytes < sizeof(T))
                {
                    _leftBytes = 128 * 1024;
                    //_memory = (char*)malloc(_leftBytes);
                    _memory = (char*)SystemAlloc(_leftBytes);
                    if (_memory == nullptr)
                    {
                        throw std::bad_alloc();
                    }
                }
                obj = (T*)_memory;
                size_t objSize = sizeof(T) < sizeof(void*) ? sizeof(void*) :       //其实做了取舍
                    sizeof(T);
                _memory += objSize;
                _leftBytes -= objSize;
            }
       

            new(obj)T; // 使?定位 new调? T的构造函数初始化
            return obj;
    }
    void Delete(T* obj)
    {
        // 
        显?调?的
            T
            的析构函数进?清理

            obj->~T();
        // 
        头插到
            freeList
            * ((void**)obj) = _freeList;
        _freeList = obj;
    }
private:
    char* _memory = nullptr;
    int   _leftBytes = 0;
    // 
    指向内存块的指针

        // 
        内存块中剩余字节数

        void* _freeList = nullptr;  // 
    管理还回来的内存对象的?由链表

};
