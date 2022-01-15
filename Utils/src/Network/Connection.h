#pragma once
#include <memory>
#include <string>
#include <map>
#include <iostream> // temporary log util

#include "../dllcommon.h"
#include "Socket.h"

namespace Network
{
	class SocketListener;

	class Connection
	{
		friend class NetworkController;
	public:
		enum class EXPORT EventType
		{
			None,
			Disconnect,
			Message,
			TerminateCalculation,
		};
		enum class EXPORT OperationResult
		{
			Success,
			Timeout,
			Failure
		};

		Connection() :
			m_socket(INVALID_SOCKET), m_openSocketFlag(false) { }
		Connection(SOCKET socket) :
			m_socket(socket), m_openSocketFlag(true) { }
		EXPORT virtual ~Connection();

		EXPORT bool IsOpen() const;
		EXPORT OperationResult Send(const std::string& string);
		EXPORT OperationResult TerminateCalculation();
		EXPORT OperationResult Disconnect();
		EXPORT OperationResult Receive(EventType& e, std::string& string);
		EXPORT void Close();

	private:
		OperationResult Open(const PCSTR& address, const PCSTR& port);
		OperationResult OpenImpl(const PCSTR& address, const PCSTR& port, const timeval& timeout);
		OperationResult SendImpl(const std::string& string);
		OperationResult ReceiveImpl(EventType& e, std::string& string);
		OperationResult Send(const std::string& in, const timeval& timeout);
		OperationResult Recv(std::string& out, const timeval& timeout);

		static std::map<const char, EventType> m_eventsMap;

		bool m_openSocketFlag;
		SOCKET m_socket;
		timeval m_defaultTimeout = { 5, 0 };
	};

	using EventType = Connection::EventType;
};