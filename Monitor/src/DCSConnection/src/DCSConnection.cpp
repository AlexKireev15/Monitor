#include "../DCSConnection.h"

DCSConnection::DCSConnection()
{
    m_connectThread = std::thread(&DCSConnection::_ConnectThread, this);
}

DCSConnection::~DCSConnection()
{
    m_isStop = true;
    m_connectNotified = true;
    m_connectCondition.notify_one();
    if (m_connectThread.joinable())
        m_connectThread.join();
}

void DCSConnection::Connect(const PCSTR& host, const PCSTR& port, const Callback& callback)
{
    std::unique_lock<std::mutex> locker(m_connectMutex);
    m_connectionQueue.push({ { host, port }, callback });
    m_connectNotified = true;
    m_connectCondition.notify_one();
}

std::string DCSConnection::GetName(const ConnectionInfo& info) const
{
    std::string connectName = info.m_host + ":" + info.m_port;
    std::string idxStr = "";
    int idx = 1;
    while (m_connectionMap.find(connectName + idxStr) != m_connectionMap.end())
    {
        idxStr = std::to_string(idx);
        ++idx;
    }

    return connectName + idxStr;
}

void DCSConnection::_ConnectThread()
{
    while (!m_isStop)
    {
        std::pair<ConnectionInfo, Callback> connectionInfo;
        {
            std::unique_lock<std::mutex> locker(m_connectMutex);
            while (!m_connectNotified)
            {
                m_connectCondition.wait(locker);
            }
            if (m_isStop)
                break;

            if (m_connectionQueue.empty())
                continue;
            connectionInfo = m_connectionQueue.front();
            m_connectionQueue.pop();
        }

        auto info = connectionInfo.first;
        auto connection = Network::NetworkController::GetInstance()->Connect(info.m_host.c_str(), info.m_port.c_str());

        {
            std::unique_lock<std::mutex> locker(m_connectMutex);
            if (connection && connection->IsOpen())
            {
                std::string connectName = GetName(info);
                auto pSingleConnection = std::make_shared<SingleConnection>(connectName, connection, connectionInfo.second);
                m_connectionMap.emplace(connectName, pSingleConnection);
                pSingleConnection->StartThread();
            }
        }
    }
}

DCSConnection::SingleConnection::SingleConnection(const std::string& connectionName, const std::shared_ptr<Network::Connection>& connection, const std::function<void(const std::string&)>& callback) :
    m_connectionName(connectionName),
    m_connection(connection),
    m_callback(callback)
{
}

DCSConnection::SingleConnection::~SingleConnection()
{
    if (m_connection && m_connection->IsOpen())
        m_connection->Close();
    if (m_thread.joinable())
        m_thread.join();
}

void DCSConnection::SingleConnection::StartThread()
{
    m_thread = std::thread(&SingleConnection::_ReceiveThread, this);
}

void DCSConnection::SingleConnection::_ReceiveThread()
{
    while (m_connection->IsOpen())
    {
        std::string str;
        Network::EventType e;
        m_connection->Receive(e, str);

        SYSTEMTIME timestamp;
        GetLocalTime(&timestamp);
        std::string outStr =
            m_connectionName + "|"
            + std::to_string(timestamp.wHour) + ":"
            + std::to_string(timestamp.wMinute) + ":"
            + std::to_string(timestamp.wSecond) + ":"
            + std::to_string(timestamp.wMilliseconds) + " : "
            + str;

        m_callback(outStr);
    }
}
