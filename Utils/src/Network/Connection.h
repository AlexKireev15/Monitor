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
			Exit,
			Message,
			StartCalculation,
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
		EXPORT std::string GetName() const;
		EXPORT void SetName(const std::string& name);
		EXPORT OperationResult Send(const std::string& string);
		EXPORT OperationResult StartCalculation();
		EXPORT OperationResult TerminateCalculation();
		EXPORT OperationResult Exit();
		EXPORT OperationResult Receive(EventType& e, std::string& string);
		EXPORT void Close();

	private:
		OperationResult Open(const PCSTR& address, const PCSTR& port);
		OperationResult OpenImpl(const PCSTR& address, const PCSTR& port, const timeval& timeout = { 5, 0 });
		OperationResult SendImpl(const std::string& in, timeval timeout = { 5, 0 });
		OperationResult ReceiveImpl(EventType& e, std::string& string);
		OperationResult RecvString(std::string& out, const timeval& timeout = { 5, 0 });

		static std::map<const char, EventType> m_eventsMap;

		bool m_openSocketFlag;
		SOCKET m_socket;
		timeval m_defaultTimeout = { 5, 0 };
		std::string m_name;
	};

	using EventType = Connection::EventType;
};