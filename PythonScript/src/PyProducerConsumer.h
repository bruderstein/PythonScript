#ifndef _PYPRODUCER_H
#define _PYPRODUCER_H

namespace NppPythonScript
{

template<typename DataT>
class PyProducerConsumer
{
public:
	PyProducerConsumer();
	virtual ~PyProducerConsumer();

	void startConsumer();
	void stopConsumer();

	bool consumerBusy();

protected:
	bool produce(const std::shared_ptr<DataT>& data);
	virtual void consume(const std::shared_ptr<DataT>& data) = 0;
	virtual void queueComplete() { };
	DWORD getConsumerThreadID() { return m_dwThreadId; };

private:
	HANDLE m_queueMutex;
	HANDLE m_dataAvailable;
	HANDLE m_shutdown;
	std::queue<std::shared_ptr<DataT> > m_queue;
	DWORD m_dwThreadId;
	HANDLE m_hConsumerThread;
	bool m_consuming;

	void consumer();

	static void threadStart(PyProducerConsumer *instance);

};

template <typename DataT>
PyProducerConsumer<DataT>::PyProducerConsumer() : 
	m_queueMutex(CreateMutex(NULL, FALSE, NULL)),
	m_dataAvailable(CreateEvent(NULL, TRUE, FALSE, NULL)),
	m_shutdown(CreateEvent(NULL, TRUE, FALSE, NULL)),
	m_dwThreadId(NULL),
	m_hConsumerThread(NULL),
	m_consuming(false)
{
}

template <typename DataT>
PyProducerConsumer<DataT>::~PyProducerConsumer()
{
	stopConsumer();
	
	CloseHandle(m_queueMutex);
	m_queueMutex = NULL;

	CloseHandle(m_dataAvailable);
	m_dataAvailable = NULL;

	CloseHandle(m_shutdown);
	m_shutdown = NULL;
}

template <typename DataT>
void PyProducerConsumer<DataT>::stopConsumer()
{
	SetEvent(m_shutdown);
	if (m_hConsumerThread)
	{
		WaitForSingleObject(m_hConsumerThread, INFINITE);
		CloseHandle(m_hConsumerThread);
		m_hConsumerThread = NULL;
	}
}

template <typename DataT>
bool PyProducerConsumer<DataT>::produce(const std::shared_ptr<DataT>& data)
{
	bool retVal = false;
	DWORD mutexResult = WaitForSingleObject(m_queueMutex, INFINITE);

	
	if (mutexResult == WAIT_OBJECT_0)
	{
		try
		{
			m_queue.push(data);
			retVal = true;
		}
		catch(...)
		{
			
		}

		ReleaseMutex(m_queueMutex);

		SetEvent(m_dataAvailable);
	}

	return retVal;
}

template<typename DataT>
void PyProducerConsumer<DataT>::startConsumer()
{
	if (!m_hConsumerThread)
	{
		m_hConsumerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadStart, this, 0, &m_dwThreadId);
	}
}

template<typename DataT>
bool PyProducerConsumer<DataT>::consumerBusy()
{
	return m_consuming;
}


template<typename DataT>
void PyProducerConsumer<DataT>::consumer()
{
	HANDLE waitHandles[] = {m_dataAvailable, m_shutdown};
	bool queueEmpty;
	for(;;)
	{
		
		DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
		
		if (waitResult == WAIT_OBJECT_0 + 1 || waitResult == WAIT_ABANDONED_0 + 1)  // Shutdown signalled
		{
			// Shutdown immediately
			// An alternative would be to "finish up" the queue
			// but we don't want to do that in PyScript - just close N++
			break;
		}
		m_consuming = true;

		DWORD queueAvailable = WaitForSingleObject(m_queueMutex, INFINITE);

		if (queueAvailable == WAIT_OBJECT_0)
		{
			queueEmpty = false;
			std::shared_ptr<DataT> data = m_queue.front();
			m_queue.pop();
			if (m_queue.empty())
			{
				queueEmpty = true;
				ResetEvent(m_dataAvailable);
			}

			ReleaseMutex(m_queueMutex);
			
			consume(data);
			
			if (queueEmpty)
			{
				waitResult = WaitForSingleObject(m_dataAvailable, 0);
				if (waitResult != WAIT_OBJECT_0)
				{
					m_consuming = false;
					queueComplete();
				}
			}
		}
	}
}

template<typename DataT>
void PyProducerConsumer<DataT>::threadStart(PyProducerConsumer *instance)
{
	instance->consumer();
}


}

#endif
