#ifndef __THREAD_H__
#define __THREAD_H__

#include <windows.h>
#include <process.h>

namespace System
{

namespace Thread
{

class Thread
{
public:
    Thread(void)
    {
        m_bRunning = false;
        m_bCreated = false;
        m_bCreateSuspended = true;
        m_bTerminated = false;
        m_bSuspended = true;
        m_hThread = NULL;
        m_errCode = 0;
    }

    virtual ~Thread(void)
    {
        //need call Destroy();
        if (m_hThread && m_hThread != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_hThread);
        }
    }
public:
    bool Resume()
    {
        if (!m_hThread || m_hThread == INVALID_HANDLE_VALUE)
            return false;
        int suspendCount = (int)ResumeThread(m_hThread);
        if (suspendCount < 0)
        {
            m_errCode = GetLastError();
            return false;
        }
        if (suspendCount == 1)
        {
            m_bSuspended = false;
        }
        return true;
    }

    bool Suspend()
    {
        if (!m_hThread || m_hThread == INVALID_HANDLE_VALUE)
            return false;
        bool oldSuspend = m_bSuspended;
        m_bSuspended = true;
        if ((int)(SuspendThread(m_hThread)) < 0)
        {
            m_errCode = GetLastError();
            m_bSuspended = oldSuspend;
            return false;
        }
        else
        {
            return true;
        }
    }

    typedef unsigned int (__stdcall* _THREAD_FUN)(void*);

    bool Create(bool createSuspended = true)
    {
        if (m_bCreated)
            return false;
        m_bCreateSuspended = createSuspended;
        m_bTerminated = false;
        if (m_bCreateSuspended)
        {
            m_hThread = (HANDLE)_beginthreadex(NULL, 0, (_THREAD_FUN)ThreadProc,(void*)this, CREATE_SUSPENDED,(unsigned int*)&m_threadID );
        }
        else
        {
            m_hThread = (HANDLE)_beginthreadex(NULL, 0, (_THREAD_FUN)ThreadProc,(void*)this, 0, (unsigned int*)&m_threadID );
        }
        if (m_hThread && m_hThread != INVALID_HANDLE_VALUE)
        {
            m_bCreated = true;
            if (m_bCreateSuspended)
            {
                m_bSuspended = true;
            }
            else
            {
                m_bSuspended = false;
            }
            return true;
        }
        else
        {
            m_errCode = GetLastError();
            return false;
        }
    }

    bool Destroy()
    {
        if (!m_hThread || m_hThread == INVALID_HANDLE_VALUE)
            return false;
        if (m_bCreated)
        {
            Terminate();
            while (m_bSuspended)
            {
                Resume();
            }
            WaitForEnd();
            CloseHandle(m_hThread);
            m_hThread = NULL;
            m_bCreated = false;
            return true;
        }
        return false;
    }

    void Terminate()
    {
        m_bTerminated = true;
    }

    bool IsRunning()
    {
        return (m_bRunning && (m_hThread != NULL) && (m_hThread != INVALID_HANDLE_VALUE));
    }

    HANDLE GetHandle() const
    {
        return m_hThread;
    }

    DWORD GetLastError() const
    {
        return m_errCode;
    }

    DWORD WaitForEnd(unsigned long timeout = INFINITE)
    {
        if (!m_hThread || m_hThread == INVALID_HANDLE_VALUE)
            return 0;
        if (m_bCreated)
        {
            if (m_bSuspended)
            {
                while (m_bSuspended)
                {
                    Resume();
                }
                return WaitForSingleObject(m_hThread, timeout);
            }
            else
                return WaitForSingleObject(m_hThread, timeout);
        }
        return 0;
    }

    bool SetPriority(int priority)
    {
        if (m_hThread)
        {
            return (::SetThreadPriority(m_hThread, priority) != FALSE);
        }
        return false;
    }

    virtual UINT Run() = 0;
protected:
    static UINT ThreadProc(HANDLE pThread)
    {
        UINT ret = NULL;
        ((Thread*) pThread)->m_bRunning = true;
        if (!((Thread*) pThread)->m_bTerminated)
        {
            ret = ((Thread*)pThread)->Run();
        }
        ((Thread*) pThread)->m_bRunning = false;
        return ret;
    }
protected:
    bool m_bCreateSuspended;
    bool m_bRunning;
    bool m_bCreated;
    bool m_bTerminated;
    bool m_bSuspended;
    HANDLE m_hThread;
    DWORD m_errCode;
    unsigned long m_threadID;
protected:
    Thread& operator=(const Thread& /*rhs*/)
    {
        return *this;
    }

    Thread(const Thread& /*rhs*/)
    {
        return;
    }
};

}//Thread
}//System

#endif
