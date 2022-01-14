#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <queue>

#include <Network.h>

class MonitorConnection
{
public:
    MonitorConnection() { }
	virtual ~MonitorConnection();

	bool StartListening(const PCSTR& host, const PCSTR& port);
	void Send(const std::string& message);

private:
	void _ThreadMain();

	std::shared_ptr<Network::SocketListener> m_listener;
	
	std::thread m_sendThread;
	std::mutex m_sendMutex;
	std::condition_variable m_sendCondition;
	bool m_notified = false;
	bool m_isStop = false;

	std::queue<std::string> m_messages;
};