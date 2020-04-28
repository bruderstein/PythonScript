#ifndef GILMANAGER_20140215_H
#define GILMANAGER_20140215_H

#include "DebugTrace.h"

namespace NppPythonScript
{
    /*

    This whole schenanigans is necessary, as you can't call PyGILState_Ensure() from a thread that already has the GIL.

    For the ScintillaWrapper (specifically, but not exclusively), it's tricky to impossible to tell if we need to release the GIL. If we're running on the 
    Python worker thread, and we make a call to Scintilla, that triggers a callback, we need to give it up (in order that the callback can run).  The callback
    identification must be performed with the GIL, so it would block if we didn't give it up. (There's an optimisation that if no callbacks are registered, then it
    doesn't check, and doesn't acquire the GIL)
	
	However, if we're being called from the replace function, and therefore the main thread, we probably don't even have the GIL to give up.
	
	With the different threads running the different callbacks, and python code running on a thread (*generally* - see the editor.replace() suite)
    it has become impossible to manage manually (read: the Wrong Way To Do It(tm)).  These objects manage the GIL, and provide a easy, safe way to ensure 
	you have the GIL when you need it, and give it up correctly whatever happens in the mean time.

    The doIHaveTheGIL() function is taken from Python 3.4.0, Issue #17522. An extra workaround is included to make it work when Py_DEBUG is defined.
    - see note below where PyThreadState_GET is redefined.
    */


#ifdef Py_DEBUG
    /* For Py_DEBUG, PyThreadState_GET() is redefined to PyThreadState_Get(), which checks that the current ThreadState is not null
     * We "undo" that define here, as we need to get the current threadstate without checking if it's not set
     */
#undef PyThreadState_GET
#define PyThreadState_GET()  (_PyThreadState_Current)
#endif

    class GILHandler
	{
	protected:
		static bool doIHaveTheGIL()
		{
			//TODO check if PyThreadState_GET should be used or something else
            PyThreadState* thisThreadState = PyGILState_GetThisThreadState();
            return (thisThreadState && thisThreadState == _PyThreadState_UncheckedGet());
		}

	};


    /* GILLock holds the GIL whilst it exists.  If you already have the GIL, it will do nothing.
     * If however, you didn't already have the lock, it will obtain the lock on construction, 
     * and release it back upon destruction.
     */
    class GILLock : public GILHandler
	{
	public:
		GILLock();
		virtual ~GILLock();

	private:
        GILLock(const GILLock&); /* disable copying */
        GILLock& operator = (const GILLock&); /* disable assignment */
		PyGILState_STATE m_state;
		bool m_hasLock;
	};


    /* GILRelease will release the GIL if you have it for as long as the object exists.
     * On destruction it will reacquire the lock.  You can reacquire the GIL sooner than
     * destruction by calling reacquire(). This makes the destructor a no-op.
     */
    class GILRelease : public GILHandler
	{
	public:
        GILRelease();
        virtual ~GILRelease();

        /** Reacquire the GIL after releasing it 
         * This is performed automatically on destruction of this object, but if 
         * you need to reacquire the lock before the object is disposed, then use this method
         * After usage of this method, the destructor becomes a no-op
         */
        void reacquire();

	private:
        GILRelease(const GILRelease&);
        GILRelease& operator = (const GILRelease&); /* disable assignment */
        PyThreadState *m_threadState;
		bool m_lockReleased;
	};


	

}
#endif // GILMANAGER_20140215_H 