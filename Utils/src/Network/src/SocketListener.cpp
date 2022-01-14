#include "../SocketListener.h"
#include "../Connection.h"

Network::SocketListener::~SocketListener()
{
	if (m_openSocketFlag)
	{
		closesocket(m_socket);
		m_openSocketFlag = false;
	}
}

Network::SocketListener & Network::SocketListener::Open(const PCSTR & address, const PCSTR & port)
{
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	addrinfo * info = NULL;
	int resultCode = getaddrinfo(address, port, &hints, &info);
	std::shared_ptr<addrinfo> pInfo = std::shared_ptr<addrinfo>(info, freeaddrinfo);
	if (resultCode != 0)
	{
		std::cerr << "Unable to listen " << address << ":" << port << " Error code: " << resultCode << std::endl;
		return *this;
	}

	m_socket = socket(info->ai_family, pInfo->ai_socktype, pInfo->ai_protocol);
	if (m_socket == INVALID_SOCKET)
	{
		std::cerr << "Unable to open socket. Error code: " << WSAGetLastError() << std::endl;
		return *this;
	}

	resultCode = bind(m_socket, info->ai_addr, (int)info->ai_addrlen);
	if (resultCode == SOCKET_ERROR)
	{
		std::cerr << "Unable to bind socket. Error code: " << WSAGetLastError() << std::endl;
		closesocket(m_socket);
		return *this;
	}

	resultCode = listen(m_socket, SOMAXCONN);
	if (resultCode == SOCKET_ERROR)
	{
		std::cerr << "Unable to listen socket. Error code: " << WSAGetLastError() << std::endl;
		closesocket(m_socket);
		return *this;
	}

	m_openSocketFlag = true;

	return *this;
}

bool Network::SocketListener::IsOpen() const
{
	return m_openSocketFlag;
}

std::shared_ptr<Network::Connection> Network::SocketListener::Accept()

{
	if (!m_openSocketFlag)
	{
		std::cerr << "Socket listen socket not opened" << std::endl;
		return nullptr;
	}

	SOCKET connectionSocket = accept(m_socket, NULL, NULL);
	if (connectionSocket == INVALID_SOCKET)
	{
		std::cerr << "Accept failed. Error code: " << WSAGetLastError() << std::endl;
		return nullptr;
	}
	m_connection = std::make_shared<Connection>(connectionSocket);
	return m_connection;
}

std::shared_ptr<Network::Connection> Network::SocketListener::GetConnection() const
{
	return m_connection;
}
