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

std::string DCSConnection::GetNewConnectionName(const std::string & host, const std::string & port) const
{
	unsigned idx = 0;
	std::string uniqueName = host + ":" + port;
	std::string searchName = uniqueName;

	auto findName = [&searchName](auto connection)
	{
		return connection->GetName() == searchName;
	};

	while (std::find_if(m_connections.begin(), m_connections.end(), findName) != m_connections.end())
	{
		searchName = uniqueName + "_" + std::to_string(++idx);
	}

	return searchName;
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
		pConnection->SetName(GetNewConnectionName(qConnection.host, qConnection.port));

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
