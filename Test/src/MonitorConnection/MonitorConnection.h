#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <queue>
#include <list>

#include <Network.h>

class MonitorConnection
{
	using Callback = std::function<void(const Network::Connection::EventType& e, const std::string&)>;
public:
	MonitorConnection() { }
	virtual ~MonitorConnection();

	bool StartListening(const PCSTR& host, const PCSTR& port, Callback recvCallback);
	void Send(const std::string& str);

private:
	void _ThreadAccept();

	bool m_stopped = false;
	Callback m_recvCallback;
	std::shared_ptr<Network::SocketListener> m_listener;
	std::list<std::shared_ptr<Network::AsyncConnection>> m_connections;

	std::thread m_acceptThread;
}
