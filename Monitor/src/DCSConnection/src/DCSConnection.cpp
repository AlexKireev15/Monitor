#include "../DCSConnection.h"

DCSConnection::DCSConnection()
{
    m_connectThread = std::thread(&DCSConnection::_ThreadConnect, this);
}

DCSConnection::~DCSConnection()
{
    {
        std::unique_lock<std::mutex> locker(m_connectMutex);
        m_notifyConnect = true;
        m_stopped = true;
        m_connectCondition.notify_one();
    }

    if (m_connectThread.joinable())
        m_connectThread.join();
}

void DCSConnection::Connect(const PCSTR& host, const PCSTR& port, const AC::SendCallback& sendCallback, const AC::ReceiveCallback& recvCallback, const ConnectCallback& connectCallback, const AC::CloseCallback& closeCallback)
{
    std::unique_lock<std::mutex> locker(m_connectMutex);
    m_connectionQueue.push({host, port, sendCallback, recvCallback, connectCallback, closeCallback});
    m_notifyConnect = true;
    m_connectCondition.notify_one();
}

void DCSConnection::_ThreadConnect()
{
    while (!m_stopped)
    {
        QueuedConnection qConnection;
        {
            std::unique_lock<std::mutex> locker(m_connectMutex);
            while (!m_notifyConnect)
            {
                m_connectCondition.wait(locker);
            }

            if (m_connectionQueue.empty())
                continue;

            qConnection = m_connectionQueue.front();
            m_connectionQueue.pop();
        }

        std::shared_ptr<Network::Connection> pConnection;
        auto result = Network::NetworkController::GetInstance()->Connect(qConnection.host, qConnection.port, pConnection);
        std::shared_ptr<Network::AsyncConnection> asyncConnection;
        if (result == Network::Connection::OperationResult::Success)
        {
            asyncConnection = std::make_shared<Network::AsyncConnection>(pConnection, qConnection.recvCallback, qConnection.sendCallback, qConnection.closeCallback);
            asyncConnection->Start();
            m_connections.push_back(asyncConnection);
        }

        qConnection.connectCallback(result, asyncConnection);
    }

}
