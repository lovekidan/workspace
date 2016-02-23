#ifndef _CRITICAL_SECTION_HPP_
#define _CRITICAL_SECTION_HPP_

#ifdef _WINDOWS
#include <windows.h>
#elif defined _UNIX
#include <pthread.h>
#endif

namespace System
{

namespace Thread
{

class CriticalSection
{
public:
    CriticalSection()
    {
#ifdef _WINDOWS
        ::InitializeCriticalSection(&m_cs);
#elif defined _UNIX
        ::pthread_mutex_init(&m_mutex, NULL);
#endif
    }
    ~CriticalSection()
    {
#ifdef _WINDOWS
        ::DeleteCriticalSection(&m_cs);
#elif defined _UNIX
        ::pthread_mutex_destroy(&m_mutex);
#endif
    }
public:
    inline void Lock()
    {
#ifdef _WINDOWS
        ::EnterCriticalSection(&m_cs);
#elif defined _UNIX
        ::pthread_mutex_lock(&m_mutex);
#endif
    }
    inline void Unlock()
    {
#ifdef _WINDOWS
        ::LeaveCriticalSection(&m_cs);
#elif defined _UNIX
        ::pthread_mutex_unlock(&m_mutex);
#endif
    }
protected:
#ifdef _WINDOWS
    CRITICAL_SECTION m_cs;
#elif defined _UNIX
    pthread_mutex_t  m_mutex;
#endif
protected:
    CriticalSection(const CriticalSection& /*rhs*/) {}
    CriticalSection& operator=(const CriticalSection& /*rhs*/)
    {
        return *this;
    }
};

class CriticalSectionGuard
{
public:
    CriticalSectionGuard(CriticalSection& theCS) : m_cs(theCS) {}
    ~CriticalSectionGuard()
    {
        m_cs.Unlock();
    }
protected:
    CriticalSection&  m_cs;
protected:
    CriticalSectionGuard(const CriticalSectionGuard& /*rhs*/) : m_cs(*(new CriticalSection())) {}
    const CriticalSectionGuard& operator=(const CriticalSectionGuard& /*rhs*/)
    {
        return *this;
    }
};

}//namespace Thread

}//namespace System

#endif