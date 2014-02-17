#include "stdafx.h"
#include "GILManager.h"

namespace NppPythonScript
{
    GILManager* GILManager::s_gilManagerInstance = NULL;

     GILLock::GILLock(GILManager* manager, bool takeLock)
				 : m_manager(manager),
                   m_hasLock(takeLock)
     {
         if (takeLock)
         {
             DEBUG_TRACE(L"Acquiring GIL...\n");
             m_state = PyGILState_Ensure();
             DEBUG_TRACE(L"GIL Acquired.\n");
             m_manager->setThreadWithGIL();
             DEBUG_TRACE(L"ThreadID set after GIL acquired");
         }
     }

	 GILLock::~GILLock() 
     {
         // Identify to the manager that we're giving it up. 
         // This must be done first, as there could be someone waiting on an PyGILState_Ensure(), and as soon 
         // as we do a PyGILState_Release(), the new thread will take the GIL and set the new thread ID in the manager.
         // There's a yummy race condition for ya, right there.
         // Doing this way round, any other thread that comes in between these two calls, gets "you don't have the GIL", which is correct.
         // The only thread that would get the wrong answer is *THIS* thread, which would (theoretically) get "you don't have the GIL", which is wrong until the 
         // second call (PyGILState_Release) is called. However, *this* thread is here, so that ain't gonna happen.
         if (m_hasLock)
         {
             DEBUG_TRACE(L"Releasing GIL...\n");
             m_manager->release();
             PyGILState_Release(m_state);
             DEBUG_TRACE(L"GIL Released.\n");
         }
     }


     GILRelease::GILRelease(GILManager* manager, bool releaseLock)
		 : m_manager(manager),
           m_lockReleased(releaseLock)
	 {
         if (releaseLock)
		 {
             DEBUG_TRACE(L"Temp GIL Release requested\n");
             m_manager->release();
			 m_threadState = PyEval_SaveThread();
		 }
	 }

     GILRelease::~GILRelease()
	 {
         if (m_lockReleased)
		 {
             DEBUG_TRACE(L"Re-acquiring GIL after temporary release\n");
             PyEval_RestoreThread(m_threadState);
             DEBUG_TRACE(L"GIL reacquired after temporary release\n");
			 // ::Sleep(250);
             DEBUG_TRACE(L"Sleep after GIL reacquired complete, setting threadid in manager\n");
             m_manager->setThreadWithGIL();
             DEBUG_TRACE(L"Manager updated with new thread ID\n");
		 }
	 }

     void GILRelease::reacquire()
	 {
         if (m_lockReleased)
		 {
             m_lockReleased = false;
             PyEval_RestoreThread(m_threadState);
		 }
	 }
}