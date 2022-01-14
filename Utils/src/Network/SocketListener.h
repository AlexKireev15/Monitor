#pragma once
#include <iostream> // temporary log util

#include "../dllcommon.h"
#include "Socket.h"

namespace Network
{
	class Connection;
	class SocketListener
	{
	public:
		SocketListener() : m_openSocketFlag(false) { }
		EXPORT ~SocketListener();
		EXPORT SocketListener& Open(const PCSTR& address, const PCSTR& port);
		EXPORT bool IsOpen() const;
		EXPORT std::shared_ptr<Connection> Accept();
		EXPORT std::shared_ptr<Connection> GetConnection() const;
		
	private:
		bool m_openSocketFlag;
		SOCKET m_socket;
		std::shared_ptr<Connection> m_connection;
	};
};