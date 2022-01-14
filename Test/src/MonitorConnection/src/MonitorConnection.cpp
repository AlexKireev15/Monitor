#include "../MonitorConnection.h"

bool MonitorConnection::StartListening(const PCSTR& host, const PCSTR& port)
{
	m_listener = Network::NetworkController::GetInstance()->Listen(host, port);
	if (!m_listener->IsOpen())
	{
		return false;
	}

	m_sendThread = std::thread(&MonitorConnection::_ThreadMain, this);

	return true;
}

void MonitorConnection::Send(const std::string& message)
{
	std::unique_lock<std::mutex> locker(m_sendMutex);
	m_messages.push(message);
	m_notified = true;
	m_sendCondition.notify_one();
}

MonitorConnection::~MonitorConnection()
{
	{
		std::unique_lock<std::mutex> locker(m_sendMutex);
		m_isStop = true;
		m_notified = true;
		m_sendCondition.notify_one();
	}

	if (m_sendThread.joinable())
		m_sendThread.join();
}

void MonitorConnection::_ThreadMain()
{
	{
		std::unique_lock<std::mutex> locker(m_sendMutex);
		std::shared_ptr<Network::Connection> pConnection;
		unsigned int attemptsNumber = 5;
		for (auto idx = 0; idx < attemptsNumber; ++idx)
		{
			pConnection = m_listener->Accept();
			if (pConnection->IsOpen())
			{
				break;
			}
		}
		if (!pConnection->IsOpen())
		{
			return;
		}
	}

	auto pConnection = m_listener->GetConnection();
	while (!m_isStop)
	{
		std::unique_lock<std::mutex> locker(m_sendMutex);
		while (!m_notified)
		{
			m_sendCondition.wait(locker);
		}
		if (m_isStop)
			break;

		while (!m_messages.empty())
		{
			pConnection->Send(m_messages.front());
			m_messages.pop();
		}
		m_notified = false;
	}
}