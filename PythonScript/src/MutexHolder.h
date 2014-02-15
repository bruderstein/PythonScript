namespace NppPythonScript
{
    class MutexHolder 
	{
	public:
		MutexHolder(HANDLE mutex)
			: m_mutex(mutex)
		{
			DEBUG_TRACE(L"Waiting for mutex\n");
			DWORD result = ::WaitForSingleObjectEx(mutex, INFINITE, FALSE);
            DEBUG_TRACE(result);
            DEBUG_TRACE(L"Got mutex\n");
		}

        virtual ~MutexHolder()
		{
			::ReleaseMutex(m_mutex);
            DEBUG_TRACE(L"Released mutex");
		}

	private:
        HANDLE m_mutex;
	};
}