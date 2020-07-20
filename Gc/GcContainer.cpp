#include <src/spring/GcContainer.h>

#include <thread>

GcContainer::GcContainer()
{
    Console::out("..........1");
    thread releaseObjThread([this]() {
        while (true)
        {
            {
                lock_guard<mutex> lockGuard(m_mutex);

                Console::out("gc start.......  Container size = ", m_objAddrList.size());

                while (true)
                {
                    bool isReleaseMemory = false;

                    for (int i = 0; i < m_objAddrList.size(); i++)
                    {
                        HWND objAddr = m_objAddrList[i];

                        deque<HWND> &pointAddrList = m_objAddrBindPointAddrMap[objAddr];
                        
                        for (auto &ptrAddr : pointAddrList)
                        {
                            Console::out("ptrAddr addr = ", ptrAddr);
                        }

                        if (pointAddrList.size() == 0)
                        {
                            void *objPtr = (void *)objAddr;
                            m_typeRelaseFunMap[m_objAddrReleaseFunMap[objAddr]](objPtr);

                            for (auto it = m_objAddrList.begin(); it != m_objAddrList.end(); it++)
                            {
                                if (*it == objAddr)
                                {
                                    m_objAddrList.erase(it);
                                    break;
                                }
                            }

                            for (auto it = m_objAddrBindPointAddrMap.begin(); it != m_objAddrBindPointAddrMap.end(); it++)
                            {
                                if (it->first == objAddr)
                                {
                                    m_objAddrBindPointAddrMap.erase(it);
                                    break;
                                }
                            }

                            for (auto it = m_objAddrReleaseFunMap.begin(); it != m_objAddrReleaseFunMap.end(); it++)
                            {
                                if (it->first == objAddr)
                                {
                                    m_objAddrReleaseFunMap.erase(it);
                                    break;
                                }
                            }

                            isReleaseMemory = true;
                            break;
                        }
                    }

                    if (isReleaseMemory == false)
                    {
                        break;
                    }
                }

                Console::out("gc end.......  Container size = ", m_objAddrList.size());
            }

            Sleep(1000);
        }
    });

    releaseObjThread.detach();
}

GcContainer *GcContainer::m_instance = nullptr;

map<string, std::function<void(void *)>> GcContainer::m_typeRelaseFunMap;

void GcContainer::initContainer()
{
    m_instance = new GcContainer();

    GC_ADD_TYPE(int)
    GC_ADD_TYPE(double)
    GC_ADD_TYPE(string)
    GC_ADD_TYPE(long)
}

GcContainer *GcContainer::getInstance()
{
    return m_instance;
}