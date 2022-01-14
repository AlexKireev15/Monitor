#include "../Network.h"
#include "../SocketListener.h"
#include "../Connection.h"

#include <algorithm>

using namespace Network;

std::once_flag NetworkController::m_flag;
std::unique_ptr<NetworkController> NetworkController::m_instance;

void printErrorMessage(const char message[], int resultCode)
{
	std::cerr << message << " Error code: " << resultCode << std::endl;
}

const std::unique_ptr<NetworkController>& NetworkController::GetInstance()
{
	if (!m_instance)
	{
		std::call_once(m_flag, []
		{
			Initialize();
		});
	}

	return m_instance;
}

NetworkController::NetworkController()
{
	int resultCode = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (resultCode != 0)
	{
		printErrorMessage("WSAStartup failed", resultCode);
		return;
	}
}

NetworkController::~NetworkController()
{
	//m_connections.clear();
	//m_socketListeners.clear();
	WSACleanup();
}

void Network::NetworkController::Initialize()
{
	m_instance = std::unique_ptr<NetworkController>(new NetworkController);
}

std::shared_ptr<SocketListener> Network::NetworkController::Listen(const PCSTR & address, const PCSTR & port)
{
	auto listener = std::make_shared<SocketListener>();
	if (!listener->Open(address, port).IsOpen())
	{
		return nullptr;
	}

	//m_socketListeners.push_back(listener);
	return listener;
}

std::shared_ptr<Connection> Network::NetworkController::Connect(const PCSTR & address, const PCSTR & port)
{
	auto connection = std::make_shared<Connection>();
	if (!connection->Open(address, port).IsOpen())
	{
		return nullptr;
	}

	/*{
		std::unique_lock<std::mutex> locker(m_connectionListMutex);
		m_connections.push_back(connection);
	}*/
	return connection;
}

//void Network::NetworkController::Broadcast(const std::string str)
//{
//	{
//		std::unique_lock<std::mutex> locker(m_connectionListMutex);
//		for (auto connection : m_connections)
//		{
//			if (connection->IsOpen())
//			{
//				connection->Send(str);
//			}
//		}
//	}
//	for (auto listener : m_socketListeners)
//	{
//		if (listener->IsOpen())
//		{
//			auto connection = listener->GetConnection();
//			if (connection->IsOpen())
//			{
//				connection->Send(str);
//			}
//		}
//	}
//}

//void NetworkController::NotifyClose(Network::Own* pOwn)
//{
//	if (!pOwn)
//		return;
//
//	Connection* pConnection = dynamic_cast<Connection*>(pOwn);
//	if (pConnection)
//	{
//		auto it = std::find_if(m_connections.begin(), m_connections.end(), [&pConnection](auto& c)
//			{
//				return c.get() == pConnection;
//			});
//		if (it != m_connections.end())
//		{
//			std::unique_lock<std::mutex> locker(m_connectionListMutex);
//			m_connections.erase(it);
//		}
//	}
//}
