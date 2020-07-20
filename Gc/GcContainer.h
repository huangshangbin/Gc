#pragma once

#include<library/system/Console.hpp>
#include<library/dataStruct/SafeDeque.hpp>


#include<src/spring/GcObject.h>

#include<windows.h>

#include<deque>
#include<map>
#include <string>
#include <functional>


#include<iostream>

using namespace std;


#define GC_NEW(type)\
GcContainer::getInstance()->newObj<type>(#type);

#define GC_ADD_TYPE(type)\
GcContainer::m_typeRelaseFunMap[#type] = [](void *ptr) {\
    type *testPtr = (type *)ptr;\
    delete testPtr;\
  };


/*
1 m_objAddrList 保存所有对象的地址。不会重复。

2 m_objAddrBindPointAddrMap 保存指向对象的指针(用对象包裹)，指针不会重复，不然析构的时候只会去掉一个指针。
                            如果指针列表为空，那么说明没有对象引用，应该释放掉。

3 m_objAddrReleaseFunMap 保存对象的释放函数标识

4 m_typeRelaseFunMap 类型名称绑定的对象释放函数。
*/

class GcContainer
{
private:
    deque<HWND> m_objAddrList;
    map<HWND, deque<HWND>> m_objAddrBindPointAddrMap;
    map<HWND, string> m_objAddrReleaseFunMap;

    mutex m_mutex;

private:
    GcContainer();

public:
    static GcContainer* m_instance;
    static void initContainer();
    static GcContainer* getInstance();

    static map<string, std::function<void(void*)>> m_typeRelaseFunMap;

public:
    template<typename T>
    T* newObj(string typeName)
    {
        lock_guard<mutex> lockGuard(m_mutex);

        T* obj = new T();

        m_objAddrList.push_back((HWND)obj);

        deque<HWND> ptrAddrList;
        m_objAddrBindPointAddrMap[(HWND)obj] = ptrAddrList;

        m_objAddrReleaseFunMap[(HWND)obj] = typeName;

        return obj;
    }

    void removeObjPtr(HWND objAddr, HWND ptrAddr)
    {
        lock_guard<mutex> lockGuard(m_mutex);

        bool isExistObj = false;
        for(auto& tempObjAddr : m_objAddrList)
        {
            if(tempObjAddr == objAddr)
            {
                isExistObj = true;
                break;
            }
        }

        if(isExistObj == false)
        {
            return;
        }

        deque<HWND>& ptrAddrList = m_objAddrBindPointAddrMap[objAddr];
        for(auto it = ptrAddrList.begin(); it != ptrAddrList.end(); it++)
        {
            if(*it == ptrAddr)
            {
                ptrAddrList.erase(it);
                break;
            }
        }
    }

    void addObjPtr(HWND objAddr, HWND ptrAddr)
    {
        lock_guard<mutex> lockGuard(m_mutex);

        bool isExistObj = false;
        for(auto& tempObjAddr : m_objAddrList)
        {
            if(tempObjAddr == objAddr)
            {
                isExistObj = true;
                break;
            }
        }

        if(isExistObj == false)
        {
            return;
        }

        deque<HWND>& ptrAddrList = m_objAddrBindPointAddrMap[objAddr];
        ptrAddrList.push_back(ptrAddr);
    }

private:
    
};