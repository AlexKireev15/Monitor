#include "../AsyncConnection.h"

#include <exception>

using namespace Network;
using OpResult = Connection::OperationResult;

inline void Network::AsyncConnection::Await(Lock& locker, bool& notified, std::condition_variable& condition)
{
    while (!notified)
    {
        condition.wait(locker);
    }
    notified = false;
}

void Network::AsyncConnection::AwaitRecv(Lock& locker)
{
    Await(locker, m_recvNotified, m_recvCondition);
}

void Network::AsyncConnection::AwaitSend(Lock& locker)
{
    Await(locker, m_sendNotified, m_sendCondition);
}

bool Network::AsyncConnection::CheckConnection()
{
    if (!m_connection || !m_connection->IsOpen())
        return false;
}

inline void Network::AsyncConnection::NotifyOne(bool& notified, std::condition_variable& condition)
{
    notified = true;
    condition.notify_one();
}

inline void Network::AsyncConnection::NotifyRecv()
{
    NotifyOne(m_recvNotified, m_recvCondition);
}

inline void Network::AsyncConnection::NotifySend()
{
    NotifyOne(m_sendNotified, m_sendCondition);
}

Network::AsyncConnection::~AsyncConnection()
{
    m_stopped = true;

    NotifyRecv();
    NotifySend();

    if (m_recvThread.joinable())
        m_recvThread.join();
    if (m_sendThread.joinable())
        m_sendThread.join();
}

void Network::AsyncConnection::Send(const std::string& message)
{
    Start();
    Lock locker(m_sendMutex);
    m_sendQueue.push({Connection::EventType::Message, message});
    NotifySend();
}

void Network::AsyncConnection::Terminate()
{
    Start();
    Lock locker(m_sendMutex);
    m_sendQueue.push({ Connection::EventType::TerminateCalculation, "" });
    NotifySend();
}

void Network::AsyncConnection::Start()
{
    if (m_started)
        return;

    if (!m_connection)
        throw std::exception("Nullptr connection passed to async connection");
    if (!m_connection->IsOpen())
        throw std::exception("Closed connection passed to async connection");

    m_recvThread = std::thread(&AsyncConnection::_RecvThread, this);
    m_sendThread = std::thread(&AsyncConnection::_SendThread, this);

    m_started = true;
}

void Network::AsyncConnection::_RecvThread()
{
    while (!m_stopped)
    {
        if (!CheckConnection())
        {
            m_closeCallback();
            break;
        }
        
        std::string str;
        Network::EventType e;
        if (m_connection->Receive(e, str) != Network::Connection::OperationResult::Success)
        {
            continue;
        }

        m_recvCallback(e, str);
    }
}

void Network::AsyncConnection::_SendThread()
{
    while(!m_stopped)
    {
        std::pair<Connection::EventType, std::string> sendString;
        {
            Lock locker(m_sendMutex);
            AwaitSend(locker);
            if (m_stopped)
                return;

            if (m_sendQueue.empty())
                continue;

            sendString = m_sendQueue.front();
            m_sendQueue.pop();
        }

        if (!CheckConnection())
            break;
        unsigned attemptsCount = 5;
        auto result = OpResult::Timeout;
        auto operation = sendString.first;
        for (auto idx = 0u; idx < attemptsCount && result == OpResult::Timeout; ++idx)
        {
            switch (operation)
            {
            case Connection::EventType::Message:
                result = m_connection->Send(sendString.second);
                break;
            case Connection::EventType::TerminateCalculation:
                result = m_connection->TerminateCalculation();
                break;
            default:
                break;
            }
        }

        m_sendCallback(result);
    }
}
