#pragma once
#include "../MonitorConnection/MonitorConnection.h"

#include <memory>
#include <functional>
#include <thread>
#include <mutex>

using Callable = std::function<void(void)>;

class TestWorker
{
public:
	TestWorker();
	void AddJob(Callable callable);
	void Stop();
	virtual ~TestWorker();

private:
	void _ThreadMain();

	std::thread m_workerThread;
	std::queue<Callable> m_jobsQueue;
	std::mutex m_jobsQueueMutex;
	std::condition_variable m_jobsQueueCondition;
	bool m_jobsQueueNotified = false;
	bool m_stopped = false;
};
