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
    using Callback = std::function<void(const std::string&)>;
public:
    DCSConnection();
    virtual ~DCSConnection();

    void Connect(const PCSTR& host, const PCSTR& port, const Callback& callback);

private:
    struct ConnectionInfo
    {
        std::string m_host;
        std::string m_port;
    };

    class SingleConnection
    {
    public:
        SingleConnection(const std::string& connectionName,
                         const std::shared_ptr<Network::Connection>& connection,
                         const std::function<void(const std::string&)>& callback);
        virtual ~SingleConnection();

        void StartThread();

    private:
        void _ReceiveThread();

        std::string m_connectionName;
        std::shared_ptr<Network::Connection> m_connection;
        std::function<void(const std::string&)> m_callback;
        std::thread m_thread;
    };

    std::string GetName(const ConnectionInfo& info) const;
    void _ConnectThread();

    std::thread m_connectThread;
    std::mutex m_connectMutex;
    std::condition_variable m_connectCondition;
    bool m_connectNotified = false;
    bool m_isStop = false;

    std::queue<std::pair<ConnectionInfo, Callback>> m_connectionQueue;
    std::map<std::string, std::shared_ptr<SingleConnection>> m_connectionMap;
};