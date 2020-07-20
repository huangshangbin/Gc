#pragma once

#include<src/spring/GcContainer.h>

template<typename T>
class GcObject
{
private:
    T* m_obj = nullptr;

public:
    GcObject(){ m_obj = nullptr; }

    GcObject(T* obj)
    {
        m_obj = obj;

        GcContainer::getInstance()->addObjPtr((HWND)m_obj, (HWND)this);
    }

    ~GcObject()
    {
        Console::out("~GcObject");
        release();
    }

    GcObject(const GcObject<T>& gcObj)
    {
        *this = gcObj;
    }

    GcObject(GcObject<T>&& gcObj)
    {
        *this = std::move(gcObj);
    }


    void operator = (const GcObject<T>& gcObj)
    {
        release();

        m_obj = gcObj.m_obj;

        GcContainer::getInstance()->addObjPtr((HWND)m_obj, (HWND)this);
    }

    void operator = (GcObject<T>&& gcObj)
    {
        if(this == &gcObj)
        {
            return;
        }

        release();

        m_obj = gcObj.m_obj;

        gcObj.m_obj = nullptr;
        GcContainer::getInstance()->removeObjPtr((HWND)m_obj, (HWND)&gcObj);

        GcContainer::getInstance()->addObjPtr((HWND)m_obj, (HWND)this);
    }


public:
    T* operator ->()
    {
        return m_obj;
    }

    T& operator *()
    {
        return *m_obj;
    }


private:
    void release()
    {
        if(m_obj != nullptr)
        {
            GcContainer::getInstance()->removeObjPtr((HWND)m_obj, (HWND)this);
        }
    }
};