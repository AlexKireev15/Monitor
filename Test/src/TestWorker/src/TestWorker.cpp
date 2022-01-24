#include "../TestWorker.h"

TestWorker::TestWorker()
{
	m_workerThread = std::thread(&TestWorker::_ThreadMain, this);
}

void TestWorker::AddJob(Callable callable)
{
	std::unique_lock<std::mutex> locker(m_jobsQueueMutex);
	m_jobsQueue.push(callable);
	m_jobsQueueNotified = true;
	m_jobsQueueCondition.notify_one();
}

void TestWorker::Stop()
{
	{
		std::unique_lock<std::mutex> locker(m_jobsQueueMutex);
		m_stopped = true;
		m_jobsQueueNotified = true;
		m_jobsQueueCondition.notify_one();
	}

	if (m_workerThread.joinable())
		m_workerThread.join();
}

TestWorker::~TestWorker()
{
	Stop();
}

void TestWorker::_ThreadMain()
{
	while (!m_stopped)
	{
		Callable callable;
		{
			std::unique_lock<std::mutex> locker(m_jobsQueueMutex);
			if (m_jobsQueue.empty())
			{
				while (!m_jobsQueueNotified)
				{
					m_jobsQueueCondition.wait(locker);
				}
				m_jobsQueueNotified = false;
			}

			if (m_jobsQueue.empty())
				continue;

			callable = m_jobsQueue.front();
			m_jobsQueue.pop();
		}

		callable();
	}
}
