#pragma once

#include "GUI.h"

namespace GUI
{
	class ConnectionWindow : public Element
	{
	public:
		ConnectionWindow(const std::string& host, const std::string& port, bool connectOnOpen = true) :
			m_host(host), m_port(m_port), m_connectOnOpen(connectOnOpen) { }

		void Show() override;

	private:
		std::string m_host;
		std::string m_port;
		bool m_connectOnOpen;
	};
};