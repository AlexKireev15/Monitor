#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <list>
#include <mutex>
#include <iostream> // temporary log util

#include "../dllcommon.h"
#include "Socket.h"
#include "Connection.h"

namespace Network
{
	class Connection;
	class SocketListener;

	class NetworkController
	{
		friend struct std::default_delete<NetworkController>;
	public:
		EXPORT static const std::unique_ptr<NetworkController>& GetInstance();

		EXPORT std::shared_ptr<SocketListener> Listen(const PCSTR& address, const PCSTR& port);
		EXPORT Connection::OperationResult Connect(const PCSTR& address, const PCSTR& port, std::shared_ptr<Connection>& pConnection);

	protected:
		NetworkController();
		virtual ~NetworkController();

		static void Initialize();

	private:
		WSADATA m_wsaData;

		static std::unique_ptr<NetworkController> m_instance;
		static std::once_flag m_flag;
	};

};