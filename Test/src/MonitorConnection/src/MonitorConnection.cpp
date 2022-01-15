#include "../MonitorConnection.h"

MonitorConnection::~MonitorConnection()
{
    m_stopped = true;
    if (m_acceptThread.joinable())
        m_acceptThread.join();
}

bool MonitorConnection::StartListening(const PCSTR& host, const PCSTR& port, Callback recvCallback)
{
    m_listener = Network::NetworkController::GetInstance()->Listen(host, port);
    m_recvCallback = recvCallback;

    if (!m_listener || !m_listener->IsOpen())
    {
        return false;
    }

    m_acceptThread = std::thread(&MonitorConnection::_ThreadAccept, this);

    return true;
}

void MonitorConnection::Send(const std::string& str)
{
    for (auto connection : m_connections)
    {
        connection->Send(str);
    }
}

void MonitorConnection::_ThreadAccept()
{
    while (!m_stopped)
    {
        auto pConnection = m_listener->Accept();
        if (!pConnection || !pConnection->IsOpen())
            continue;

        auto asyncConnection = std::make_shared<Network::AsyncConnection>(pConnection, m_recvCallback, [](auto){}, [](){});
        asyncConnection->Start();
        m_connections.push_back(asyncConnection);
    }
}
