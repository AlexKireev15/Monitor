#pragma once
#include <memory>
#include <string>
#include <map>
#include <iostream> // temporary log util

#include "../dllcommon.h"
#include "Socket.h"
#include "ConnectionOwn.h"

namespace Network
{
	class SocketListener;

	class Connection/* : public Own*/
	{
		friend class NetworkController;
	public:
		enum class EXPORT EventType
		{
			None,
			Disconnect,
			Ping,
			Ack,
			Message,
		};

		Connection() :
			m_socket(INVALID_SOCKET), m_openSocketFlag(false) { }
		Connection(SOCKET socket) :
			m_socket(socket), m_openSocketFlag(true) { }
		EXPORT virtual ~Connection();

		EXPORT bool IsOpen() const;
		EXPORT Connection& Send(const std::string& string);
		EXPORT Connection& Ping();
		EXPORT Connection& Disconnect();
		EXPORT Connection& Ack();
		EXPORT Connection& Receive(EventType& e, std::string& string);
		EXPORT void Close();

	private:
		Connection& Open(const PCSTR& address, const PCSTR& port);
		Connection& SendImpl(const std::string& string);
		Connection& ReceiveImpl(EventType& e, std::string& string);

		static std::map<const char, EventType> m_eventsMap;

		bool m_openSocketFlag;
		SOCKET m_socket;
	};

	using EventType = Connection::EventType;
};