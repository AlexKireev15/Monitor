#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <list>
#include <mutex>
#include <iostream> // temporary log util

#include "../dllcommon.h"
#include "Socket.h"
//#include "ConnectionOwn.h"

namespace Network
{
	class Connection;
	class SocketListener;

	class NetworkController/* : public ConnectionOwner*/
	{
		friend struct std::default_delete<NetworkController>;
	public:
		EXPORT static const std::unique_ptr<NetworkController>& GetInstance();

		EXPORT std::shared_ptr<SocketListener> Listen(const PCSTR& address, const PCSTR& port);
		EXPORT std::shared_ptr<Connection> Connect(const PCSTR& address, const PCSTR& port);
		//EXPORT void Broadcast(const std::string str);

		//void NotifyClose(Own*) override;

	protected:
		NetworkController();
		virtual ~NetworkController();

		static void Initialize();

	private:
		WSADATA m_wsaData;
		/*std::vector<std::shared_ptr<SocketListener>> m_socketListeners;
		std::list<std::shared_ptr<Connection>> m_connections;
		std::mutex m_connectionListMutex;*/

		static std::unique_ptr<NetworkController> m_instance;
		static std::once_flag m_flag;
	};

};