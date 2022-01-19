#pragma once

#include "GUI.h"

#include <string>
#include "../DCSConnection/DCSConnection.h"
#include "CmdStringsContainer.h"

namespace GUI
{
	class ConnectionWindow : public Element
	{
	public:
		ConnectionWindow(const std::string& host, const std::string& port,
			std::shared_ptr<DCSConnection> dcsConnection, bool connectOnOpen = true);

		void Show() override;
		bool IsOpened() override;
		std::string GetName() override;

	private:
		std::string m_elementName;
		std::string m_host;
		std::string m_port;
		bool m_connectOnOpen;
		std::shared_ptr<DCSConnection> m_dcsConnection;
		std::shared_ptr<Network::AsyncConnection> m_connection;
		//std::vector<std::string> m_cmdStrings;
		GUI::CmdStringsContainer m_cmdStrings;
		bool m_isOpened = true;
		bool m_isAutoScroll = true;

		std::function<void(const Network::Connection::OperationResult& result)> m_sendCallback;
		std::function<void(const Network::Connection::EventType& e, const std::string& message)> m_recvCallback;
		std::function<void(const Network::Connection::OperationResult& result, std::shared_ptr<Network::AsyncConnection> pConnection)> m_connectCallback;
		std::function<void()> m_closeCallback;

		void SendCallback(const Network::Connection::OperationResult& result);
		void RecvCallback(const Network::Connection::EventType& e, const std::string& message);
		void ConnectCallback(const Network::Connection::OperationResult& result, std::shared_ptr<Network::AsyncConnection> pConnection);
		void CloseCallback();
	};
};