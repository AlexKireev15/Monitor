#pragma once
#include <Network.h>

#include <map>
#include <string>
#include <mutex>
#include <thread>
#include <queue>
#include <functional>

#include <sysinfoapi.h>

#include "../GUI/GUI.h"

class DCSConnection
{
    using AC = Network::AsyncConnection;
    using ConnectCallback = std::function<void(const Network::Connection::OperationResult& , std::shared_ptr<Network::AsyncConnection>)>;

public:
    DCSConnection();
    virtual ~DCSConnection();

	void Connect(const PCSTR& host, const PCSTR& port,
			     GUI::Element* element,
				 const AC::SendCallback& sendCallback,
				 const AC::ReceiveCallback& recvCallback,
				 const ConnectCallback& connectCallback,
				 const AC::CloseCallback& closeCallback);
    void Exit(GUI::Element* element,
			  std::shared_ptr<Network::AsyncConnection> connection);

private:
    struct QueuedConnection
    {
        PCSTR host;
        PCSTR port;
        AC::SendCallback sendCallback;
        AC::ReceiveCallback recvCallback;
        ConnectCallback connectCallback;
        AC::CloseCallback closeCallback;
		GUI::Element* element;
    };

	std::string GetNewConnectionName(const std::string& host, const std::string& port) const;
    void _ThreadConnect();
    void _ThreadDisconnect();

    std::thread m_connectThread;
    std::thread m_disconnectThread;
    std::mutex m_connectMutex;
    std::mutex m_connectionsListMutex;
    std::mutex m_disconnectQueueMutex;
    std::condition_variable m_connectCondition;
    std::condition_variable m_disconnectCondition;
    bool m_notifyConnect = false;
    bool m_notifyDisconnect = false;
    bool m_stopped = false;
    bool m_started = false;
    std::list<QueuedConnection> m_connectionQueue;
    std::queue<std::shared_ptr<Network::AsyncConnection>> m_disconnectQueue;

    std::list<std::shared_ptr<Network::AsyncConnection>> m_connections;
};