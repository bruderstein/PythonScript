#ifndef GILMANAGER_20140215_H
#define GILMANAGER_20140215_H

#include "DebugTrace.h"

namespace NppPythonScript
{
    /*

    This whole schenanigans seems necessary, as you don't appear to be able to call PyGILState_Ensure() from a thread that already has the GIL.
    It just locks up, and digging into the code, it's waiting on a lock that the thread already has.

    For callbacks, it's impossible to know if the callback was generated from "within" python or not.
    For instance: you have a callback on scintilla SCN_MODIFIED.  If from a script you do editor.write('...'), you've got the GIL still when the callback comes.
    When the user makes a change, it calls the callback, but this time you *don't* have the GIL, so you need to get it.  
	
	With the different threads running the different callbacks, and python code running on a thread (*generally* - see the editor.replace() suite)
    it has become impossible to manage manually (read: the Wrong Way To Do It(tm)).  These objects manage the GIL, and provide a easy, safe way to ensure 
	you have the GIL when you need it, and give it up correctly whatever happens in the mean time.
    */

    class GILManager;


    /* GILLock is the generic class given back when asking for the lock
     * If the current thread already has the lock, then you'll get an instance of 
     * this class, that does nothing.
     * If however, you didn't already have the lock, you'll an instance of OwnedGILLock, which 
     * gives the lock back upon destruction.
     */

   

   
    class GILLock
	{
	public:
		GILLock(GILManager* manager, bool takeLock);
		virtual ~GILLock();

	private:
		GILManager* m_manager;
		PyGILState_STATE m_state;
		bool m_hasLock;
	};

    class GILRelease
	{
	public:
        GILRelease(GILManager *manager, bool releaseLock);
        virtual ~GILRelease();

        /** Reacquire the GIL after releasing it 
         * This is performed automatically on destruction of this object, but if 
         * you need to reacquire the lock before the object is disposed, then use this method
         * After usage of this method, the destructor becomes a no-op
         */
        void reacquire();

	private:
		GILManager* m_manager;
        PyThreadState *m_threadState;
		bool m_lockReleased;
	};

	class GILManager
	{
	public:
        static GILLock getGIL() 
		{
            if (NULL == s_gilManagerInstance)
			{
                s_gilManagerInstance = new GILManager();
			}
            return s_gilManagerInstance->getGILImpl();
		}

        static GILRelease releaseGIL()
		{
            if (NULL == s_gilManagerInstance)
			{
                s_gilManagerInstance = new GILManager();
			}
            return s_gilManagerInstance->releaseGILImpl();
		}

		void release()
		{
			::EnterCriticalSection(&m_criticalSection);
            m_currentThreadWithGIL = 0;
			::LeaveCriticalSection(&m_criticalSection);
		}

        void setThreadWithGIL()
		{
			::EnterCriticalSection(&m_criticalSection);
            m_currentThreadWithGIL = ::GetCurrentThreadId();
			::LeaveCriticalSection(&m_criticalSection);
		}

	private:

        // Disallow public construction
		GILManager() : 
           m_currentThreadWithGIL(0),
           m_tempCorruptionCheck(42)

		{
		    InitializeCriticalSection(&m_criticalSection);
		}

        // Disallow copying
		GILManager(const GILManager& );

        GILLock getGILImpl()
		{
            bool needLock;
            DWORD currentThread = ::GetCurrentThreadId();

			::EnterCriticalSection(&m_criticalSection);
            needLock = (currentThread != m_currentThreadWithGIL);
			::LeaveCriticalSection(&m_criticalSection);

            if (needLock)
			{
                // Someone else (or no-one) has the GIL, so we want to acquire it, and give it back when the object clears up
                DEBUG_TRACE(L"Thread does not have GIL, requesting\n");

			}
			else
			{
                // The current thread already has the GIL, so just give an empty object back
                DEBUG_TRACE(L"Thread already has GIL - ignoring request\n");
			}
            return GILLock(this, needLock);
		}

        GILRelease releaseGILImpl()
		{
            bool hasLock;

            DWORD currentThread = ::GetCurrentThreadId();
			::EnterCriticalSection(&m_criticalSection);
            hasLock = (currentThread == m_currentThreadWithGIL);
			::LeaveCriticalSection(&m_criticalSection);
            
            if (hasLock)
			{
                DEBUG_TRACE(L"Thread has GIL - will be released\n");
			}
			else
			{
                DEBUG_TRACE(L"Thread does not have GIL - ignoring release request (TEST - overriding)\n");
                hasLock = true;
			}

            return GILRelease(this, hasLock);

		}

        
        DWORD m_currentThreadWithGIL;
        DWORD m_tempCorruptionCheck;
        CRITICAL_SECTION m_criticalSection;
        static GILManager *s_gilManagerInstance;

	};

}
#endif // GILMANAGER_20140215_H 