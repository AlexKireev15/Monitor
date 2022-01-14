#include "../Connection.h"
#include "../SocketListener.h"
#include "../Network.h"

using namespace Network;

std::map<const char, Connection::EventType> Connection::m_eventsMap =
{
	{'D', Connection::EventType::Disconnect},
	{'P', Connection::EventType::Ping},
	{'A', Connection::EventType::Ack},
	{'M', Connection::EventType::Message},
};

Network::Connection & Network::Connection::Open(const PCSTR & address, const PCSTR & port)
{
	if (m_openSocketFlag)
	{
		std::cerr << "Socket already opened!" << std::endl;
		return *this;
	}

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	addrinfo * info = NULL;
	int resultCode = getaddrinfo(address, port, &hints, &info);
	if (resultCode != 0)
	{
		std::cerr << "Cannot resolve " << address << ":" << port << ". Error code: " << resultCode << std::endl;
		return *this;
	}
	auto addrInfo = std::shared_ptr<addrinfo>(info, freeaddrinfo);

	for (addrinfo * ptr = info; ptr != nullptr; ptr = ptr->ai_next)
	{
		m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (m_socket == INVALID_SOCKET)
		{
			std::cerr << "Cannot open socket. Error code: " << WSAGetLastError() << std::endl;
			return *this;
		}

		resultCode = connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (resultCode == SOCKET_ERROR)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	if (m_socket == INVALID_SOCKET)
	{
		std::cerr << "Cannot connect" << std::endl;
		return *this;
	}
	m_openSocketFlag = true;

	return *this;
}

bool Network::Connection::IsOpen() const
{
	return m_openSocketFlag;
}

Network::Connection& Connection::Ping()
{
	return SendImpl("P");
}

Connection& Network::Connection::Disconnect()
{
	return SendImpl("D");
}

Connection& Network::Connection::Ack()
{
	return SendImpl("A");
}

Network::Connection& Network::Connection::Send(const std::string& string)
{
	return SendImpl("M" + string);
}

Network::Connection& Network::Connection::Receive(EventType& e, std::string& string)
{
	return ReceiveImpl(e, string);
}

Connection& Network::Connection::SendImpl(const std::string& string)
{
	if (!IsOpen())
	{
		std::cerr << "Unable to send. Cause: Connection closed or not opened" << std::endl;
		return *this;
	}

	int resultCode = send(m_socket, string.c_str(), strlen(string.c_str()) + 1, 0);
	if (resultCode == SOCKET_ERROR)
	{
		Close();
		std::cerr << "Send failed. Error code: " << WSAGetLastError() << std::endl;
		return *this;
	}
	return *this;
}

Connection& Network::Connection::ReceiveImpl(EventType& e, std::string& string)
{
	if (!IsOpen())
	{
		std::cerr << "Unable to receive. Cause: Connection closed or not opened" << std::endl;
		return *this;
	}

	bool stopRecv = false;
	while (!stopRecv)
	{
		constexpr int buflen = 512;
		char recvBuf[buflen];
		int resultCode = recv(m_socket, recvBuf, buflen, 0);
		if (resultCode <= 0)
		{
			e = EventType::None;
			stopRecv = true;
			Close();
			int errCode = WSAGetLastError();
			if (errCode != 0)
				std::cout << "Unable to receive. Error code: " << errCode << std::endl;
			break;
		}

		if (resultCode > 0)
		{
			char* end = recvBuf + resultCode;
			char* pos = std::find(recvBuf, end, '\0');
			string.append(recvBuf + 1, resultCode - 1);
			char eventCh = recvBuf[0];
			if (pos != end)
			{
				if (m_eventsMap.find(eventCh) != m_eventsMap.end())
					e = m_eventsMap[eventCh];
				else
					e = EventType::None;
				stopRecv = true;
				break;
			}
		}
	}
	
	return *this;
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
				std::cerr << "Shutdown socket failed with error: " << WSAGetLastError() << std::endl;
		}
		else
		{
			closesocket(m_socket);
			//Network::NetworkController::GetInstance()->NotifyClose(this);
		}

		m_openSocketFlag = false;
	}
}

Network::Connection::~Connection()
{
	Close();
}