#include "stdafx.h"
#include "GILManager.h"

namespace NppPythonScript
{
//    GILManager* GILManager::s_gilManagerInstance = NULL;

     GILLock::GILLock()
				 : m_hasLock(false == doIHaveTheGIL())
     {
         if (m_hasLock)
         {
             DEBUG_TRACE(L"Acquiring GIL...\n");
             m_state = PyGILState_Ensure();
             DEBUG_TRACE(L"GIL Acquired.\n");
         }
     }

	 GILLock::~GILLock() 
     {
         if (m_hasLock)
         {
             DEBUG_TRACE(L"Releasing GIL...\n");
             PyGILState_Release(m_state);
             DEBUG_TRACE(L"GIL Released.\n");
         }
     }


     GILRelease::GILRelease()
		 : m_lockReleased(doIHaveTheGIL())
	 {
         if (m_lockReleased)
		 {
             DEBUG_TRACE(L"Temp GIL Release requested\n");
			 m_threadState = PyEval_SaveThread();
		 }
		 else
		 {
             DEBUG_TRACE(L"Temp GIL Release ignored- we don't have the GIL\n");
		 }

	 }

     GILRelease::~GILRelease()
	 {
         if (m_lockReleased)
		 {
             DEBUG_TRACE(L"Re-acquiring GIL after temporary release\n");
             PyEval_RestoreThread(m_threadState);
             DEBUG_TRACE(L"GIL reacquired after temporary release\n");
		 }
	 }

     void GILRelease::reacquire()
	 {
         if (m_lockReleased)
		 {
             DEBUG_TRACE(L"GILRelease::reacquire() - reacquiring GIL\n");
             PyEval_RestoreThread(m_threadState);
             DEBUG_TRACE(L"GIL Reacquired after temporary release (in reacquire())\n");
             m_lockReleased = false;
		 }
	 }
}