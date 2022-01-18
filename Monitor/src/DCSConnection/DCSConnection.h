#pragma once
#include <Network.h>

#include <map>
#include <string>
#include <mutex>
#include <thread>
#include <queue>
#include <functional>

#include <sysinfoapi.h>

class DCSConnection
{
    using AC = Network::AsyncConnection;
    using ConnectCallback = std::function<void(const Network::Connection::OperationResult& , std::shared_ptr<Network::AsyncConnection>)>;

public:
    DCSConnection();
    virtual ~DCSConnection();

    void Connect(const PCSTR& host, const PCSTR& port,
                 const AC::SendCallback& sendCallback,
                 const AC::ReceiveCallback& recvCallback,
                 const ConnectCallback& connectCallback,
                 const AC::CloseCallback& closeCallback);

private:
    struct QueuedConnection
    {
        PCSTR host;
        PCSTR port;
        AC::SendCallback sendCallback;
        AC::ReceiveCallback recvCallback;
        ConnectCallback connectCallback;
        AC::CloseCallback closeCallback;
    };

	std::string GetNewConnectionName(const std::string& host, const std::string& port) const;
    void _ThreadConnect();

    std::thread m_connectThread;
    std::mutex m_connectMutex;
    std::condition_variable m_connectCondition;
    bool m_notifyConnect = false;
    bool m_stopped = false;
    bool m_started = false;
    std::queue<QueuedConnection> m_connectionQueue;

    std::list<std::shared_ptr<Network::AsyncConnection>> m_connections;
};