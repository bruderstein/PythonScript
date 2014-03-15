#ifndef MUTEXHOLDER_20140311_H
#define MUTEXHOLDER_20140311_H
namespace NppPythonScript
{
    class MutexHolder 
	{
	public:
		MutexHolder(HANDLE mutex)
			: m_mutex(mutex)
		{
			if (mutex != NULL)
			{
				DEBUG_TRACE(L"Waiting for mutex\n");
			    ::WaitForSingleObjectEx(mutex, INFINITE, FALSE);
                DEBUG_TRACE(L"Got mutex\n");
			}
		}

        virtual ~MutexHolder()
		{
            if (m_mutex != NULL)
			{
			    ::ReleaseMutex(m_mutex);
                DEBUG_TRACE(L"Released mutex\n");
			}
		}

	private:
        HANDLE m_mutex;
	};
}

#endif // MUTEXHOLDER_20140311_H