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

	return listener;
}

Connection::OperationResult Network::NetworkController::Connect(const PCSTR& address, const PCSTR& port, std::shared_ptr<Connection>& pConnection)
{
	auto connection = std::make_shared<Connection>();
	auto result = connection->Open(address, port);
	pConnection = connection;
	return result;
}
