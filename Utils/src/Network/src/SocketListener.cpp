#include "../SocketListener.h"
#include "../Connection.h"

Network::SocketListener::~SocketListener()
{
	Stop();
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
		SOCKET_DEBUG(std::cerr << "Unable to listen " << address << ":" << port << " Error code: " << resultCode << std::endl);
		return *this;
	}

	m_socket = socket(info->ai_family, pInfo->ai_socktype, pInfo->ai_protocol);
	if (m_socket == INVALID_SOCKET)
	{
		SOCKET_DEBUG(std::cerr << "Unable to open socket. Error code: " << WSAGetLastError() << std::endl);
		return *this;
	}

	resultCode = bind(m_socket, info->ai_addr, (int)info->ai_addrlen);
	if (resultCode == SOCKET_ERROR)
	{
		SOCKET_DEBUG(std::cerr << "Unable to bind socket. Error code: " << WSAGetLastError() << std::endl);
		closesocket(m_socket);
		return *this;
	}

	resultCode = listen(m_socket, SOMAXCONN);
	if (resultCode == SOCKET_ERROR)
	{
		SOCKET_DEBUG(std::cerr << "Unable to listen socket. Error code: " << WSAGetLastError() << std::endl);
		closesocket(m_socket);
		return *this;
	}

	u_long iMode = 1;
	resultCode = ioctlsocket(m_socket, FIONBIO, &iMode);
	if (resultCode != NO_ERROR)
	{
		SOCKET_DEBUG(std::cerr << "Cannot use socket in non-blocking mode. Error code: " << resultCode << std::endl);
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
	return AcceptImpl(m_defaultTimeout);
}

void Network::SocketListener::Stop()
{
	if (m_openSocketFlag)
	{
		closesocket(m_socket);
		m_openSocketFlag = false;
	}
}

std::shared_ptr<Network::Connection> Network::SocketListener::AcceptImpl(const timeval& timeout)
{
	if (!m_openSocketFlag)
	{
		SOCKET_DEBUG(std::cerr << "Socket listen socket not opened" << std::endl);
		return nullptr;
	}

	SOCKET connectionSocket;
	bool isStopped = false;
	while (!isStopped)
	{
		connectionSocket = accept(m_socket, NULL, NULL);
		if (connectionSocket == INVALID_SOCKET)
		{
			int errorCode = WSAGetLastError();
			if (errorCode != WSAEWOULDBLOCK)
			{
				SOCKET_DEBUG(std::cerr << "Accept failed. Error code: " << errorCode << std::endl);
				return nullptr;
			}
		}
		else
		{
			isStopped = true;
			break;
		}

		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(m_socket, &fds);
		int resultCode = select(0, &fds, NULL, NULL, &timeout);
		if (resultCode == SOCKET_ERROR)
		{
			SOCKET_DEBUG(std::cerr << "Unable to determine status of socket. Error code: " << WSAGetLastError() << std::endl);
			return nullptr;
		}
		if (resultCode == 0)
		{
			return nullptr;
		}
	}

	m_connection = std::make_shared<Connection>(connectionSocket);
	return m_connection;
}

std::shared_ptr<Network::Connection> Network::SocketListener::GetConnection() const
{
	return m_connection;
}
