#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>

template <class T> 
class ObjectPool 
{
pubilc:
	T* New()
	{
		T* obj = nullptr;
		if (memory == nullptr)
		{
			memory =(char *) malloc(sizeof(T) * 128 * 1024);
			LeftBytes = sizeof(T) * 128 * 1024;
		}


		obj = (T*)memory;
		return obj;
	}

	void Delete(T * obj )
	{

		//总体分2步：
		//1.把用户还回来的内存空间里的类对象们 "T"进行析构
		//2.再将处理好的内存空间挂到freelist上，以便后续复用
		
		//调用T的析构函数
		obj -> ~T();


		//更新freelists 与 LeftBytes
		
		//遇到链表，考虑更新时：考虑链表为空和链表只有一个节点，是个好习惯(链表操作千万别死记硬背)
		
		//freelists链表为空的情况
		if (freelists == nullptr)
		{
			*freelists =*(vod **) obj;

			*((void**)obj) = _freeList;
			_freeList = obj;
		}
		else
		{
			//头插归还后的内存空间；为什么是"头插",不是"尾插"? 因为，"尾插"需要遍历freelists这个自由链表，同样解决问题，但把简单的问题复杂化了。
			*((void**)obj) = _freeList;
			_freeList = obj;

		}

			
	}




priavte:
	char* memory=nullptr;    //当前可用内存的起始地址
	int LeftBytes=0;         //当前可给外部申请的可用字节数
	char * freelists			// 以自由链表的形式管理归还后的内存空间，为的是后续可以接着复用

};
