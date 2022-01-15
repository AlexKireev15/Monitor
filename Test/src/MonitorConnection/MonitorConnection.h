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
//	using Callback = std::function<void(const Network::Connection::EventType& e, const std::string&)>;
//public:
//    MonitorConnection() { }
//	virtual ~MonitorConnection();
//
//	bool StartListening(const PCSTR& host, const PCSTR& port, Callback recvCallback);
//	void Send(const std::string& message);
//
//private:
//	void _ThreadMain();
//	void _ThreadReceive();
//
//	std::shared_ptr<Network::SocketListener> m_listener;
//	
//	std::thread m_sendThread;
//	std::thread m_recvThread;
//	std::mutex m_sendMutex;
//	std::mutex m_recvMutex;
//	std::condition_variable m_sendCondition;
//	std::condition_variable m_recvCondition;
//	bool m_recvNotified = false;
//	bool m_notified = false;
//	bool m_isStop = false;
//
//	Callback m_recvCallback;
//
//	std::queue<std::string> m_sendMessages;
};