#include "../Connection.h"
#include "../SocketListener.h"
#include "../Network.h"

using namespace Network;
using OperationResult = Connection::OperationResult;

std::map<const char, Connection::EventType> Connection::m_eventsMap =
{
	{'D', Connection::EventType::Exit},
	{'M', Connection::EventType::Message},
	{'S', Connection::EventType::StartCalculation},
	{'T', Connection::EventType::TerminateCalculation},
	{'E', Connection::EventType::Exit},
};

std::string Network::Connection::GetName() const
{
	return m_name;
}

void Network::Connection::SetName(const std::string& name)
{
	m_name = name;
}

OperationResult Network::Connection::Open(const PCSTR & address, const PCSTR & port)
{
	return OpenImpl(address, port, m_defaultTimeout);
}

OperationResult Network::Connection::OpenImpl(const PCSTR& address, const PCSTR& port, const timeval& timeout)
{
	if (m_openSocketFlag)
	{
		SOCKET_DEBUG(std::cerr << "Socket already opened!" << std::endl);
		return OperationResult::Failure;
	}

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	addrinfo* info = NULL;
	int resultCode = getaddrinfo(address, port, &hints, &info);
	if (resultCode != 0)
	{
		SOCKET_DEBUG(std::cerr << "Cannot resolve " << address << ":" << port << ". Error code: " << resultCode << std::endl);
		return OperationResult::Failure;
	}
	auto addrInfo = std::shared_ptr<addrinfo>(info, freeaddrinfo);

	for (addrinfo* ptr = info; ptr != nullptr; ptr = ptr->ai_next)
	{
		m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (m_socket == INVALID_SOCKET)
		{
			SOCKET_DEBUG(std::cerr << "Cannot open socket. Error code: " << WSAGetLastError() << std::endl);
			return OperationResult::Failure;
		}

		u_long iMode = 1;
		resultCode = ioctlsocket(m_socket, FIONBIO, &iMode);
		if (resultCode != NO_ERROR)
		{
			closesocket(m_socket);
			SOCKET_DEBUG(std::cerr << "Cannot use socket in non-blocking mode. Error code: " << resultCode << std::endl);
			return OperationResult::Failure;
		}

		resultCode = connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (resultCode == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEWOULDBLOCK)
			{
				fd_set fds;
				FD_ZERO(&fds);
				FD_SET(m_socket, &fds);
				int resultCode = select(0, NULL, &fds, NULL, &timeout);
				if (resultCode <= 0)
				{
					closesocket(m_socket);
					m_socket = INVALID_SOCKET;
					continue;
				}
			}
		}
		break;
	}

	if (m_socket == INVALID_SOCKET)
	{
		SOCKET_DEBUG(std::cerr << "Cannot connect" << std::endl);
		return OperationResult::Failure;
	}

	m_openSocketFlag = true;

	return OperationResult::Success;
}

bool Network::Connection::IsOpen() const
{
	return m_openSocketFlag;
}

OperationResult Network::Connection::TerminateCalculation()
{
	return SendImpl("T");
}

OperationResult Network::Connection::Exit()
{
	return SendImpl("E");
}

OperationResult Network::Connection::Send(const std::string& string)
{
	return SendImpl("M" + string);
}

OperationResult Network::Connection::StartCalculation()
{
	return SendImpl("S");;
}

OperationResult Network::Connection::Receive(EventType& e, std::string& string)
{
	return ReceiveImpl(e, string);
}

OperationResult Network::Connection::SendImpl(const std::string& string)
{
	return Send(string, m_defaultTimeout);
}

OperationResult Network::Connection::ReceiveImpl(EventType& e, std::string& string)
{
	std::string out;
	auto result = Recv(out, m_defaultTimeout);
	if (result == OperationResult::Success)
	{
		if(!out.empty())
		{
			if(m_eventsMap.find(out.front()) != m_eventsMap.end())
				e = m_eventsMap[out.front()];
			string.assign(++out.begin(), out.end());
		}
	}

	return result;
}

OperationResult Network::Connection::Send(const std::string& in, const timeval& timeout)
{
	if (!IsOpen())
	{
		SOCKET_DEBUG(std::cerr << "Unable to send. Cause: Connection closed or not opened" << std::endl);
		return OperationResult::Failure;
	}

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_socket, &fds);

	bool stopSend = false;
	while (!stopSend)
	{
		int resultCode = send(m_socket, in.c_str(), strlen(in.c_str()) + 1, 0);
		if (resultCode == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode != WSAEWOULDBLOCK && errorCode != WSAEMSGSIZE)
			{
				Close();
				SOCKET_DEBUG(std::cerr << "Send failed. Error code: " << errorCode << std::endl);
				stopSend = true;
				return OperationResult::Failure;
			}
		}
		else if (resultCode <= 0)
		{
			Close();
			stopSend = true;
		}
		else
		{
			stopSend = true;
			return OperationResult::Success;
		}

		resultCode = select(0, NULL, &fds, NULL, &timeout);
		if (resultCode == SOCKET_ERROR)
		{
			Close();
			SOCKET_DEBUG(std::cerr << "Unable to determine status of socket. Error code: " << WSAGetLastError() << std::endl);
			stopSend = true;
			return OperationResult::Failure;
		}
		if (resultCode == 0)
		{
			stopSend = true;
			return OperationResult::Timeout;
		}
	}

	
	return OperationResult::Success;
}

OperationResult Network::Connection::Recv(std::string& out, const timeval& timeout)
{
	if (!IsOpen())
	{
		SOCKET_DEBUG(std::cerr << "Unable to receive. Cause: Connection closed or not opened" << std::endl);
		return OperationResult::Failure;
	}

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_socket, &fds);
	
	bool stopRecv = false;
	while (!stopRecv)
	{
		constexpr int buflen = 65535;
		char recvBuf[buflen];
		int resultCode = recv(m_socket, recvBuf, buflen + 1, 0);
		if (resultCode == SOCKET_ERROR || resultCode <= 0)
		{
			int errorCode = WSAGetLastError();
			if (errorCode != WSAEWOULDBLOCK && errorCode != WSAEMSGSIZE)
			{
				Close();
				SOCKET_DEBUG(std::cerr << "Receive failed. Error code: " << errorCode << std::endl);
				stopRecv = true;
				return OperationResult::Failure;
			}
		}
		else if (resultCode > 0)
		{
			out.append(recvBuf, resultCode);
			stopRecv = true;
			return OperationResult::Success;
		}

		resultCode = select(0, &fds, NULL, NULL, &timeout);
		if (resultCode == SOCKET_ERROR)
		{
			Close();
			SOCKET_DEBUG(std::cerr << "Unable to determine status of socket. Error code: " << WSAGetLastError() << std::endl);
			stopRecv = true;
			return OperationResult::Failure;
		}
		if (resultCode == 0)
		{
			stopRecv = true;
			return OperationResult::Timeout;
		}
	}

	return OperationResult::Success;
}

void Network::Connection::Close()
{
	if (m_openSocketFlag)
	{
		int resultCode = shutdown(m_socket, SD_SEND);
		if (resultCode == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (errorCode != WSANOTINITIALISED)
				SOCKET_DEBUG(std::cerr << "Shutdown socket failed with error: " << WSAGetLastError() << std::endl);
		}
		else
		{
			closesocket(m_socket);
		}

		m_openSocketFlag = false;
	}
}

Network::Connection::~Connection()
{
	Close();
}