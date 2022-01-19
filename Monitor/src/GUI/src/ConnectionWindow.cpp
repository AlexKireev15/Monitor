#include "..\ConnectionWindow.h"

std::string GetTimestamp()
{
	SYSTEMTIME timestamp;
	GetLocalTime(&timestamp);
	auto resultedString = std::to_string(timestamp.wHour) + ":" +
		std::to_string(timestamp.wMinute) + ":" +
		std::to_string(timestamp.wSecond) + ":" +
		std::to_string(timestamp.wMilliseconds) + "|";
	for (int idx = resultedString.length(); idx < 13; ++idx)
	{
		resultedString += " ";
	}
	return resultedString;
}

std::string Format(const std::string& str)
{
	return GetTimestamp() + " " + str;
}

GUI::ConnectionWindow::ConnectionWindow(const std::string& host, const std::string& port, std::shared_ptr<DCSConnection> dcsConnection, bool connectOnOpen) :
	m_host(host), m_port(port),
	m_dcsConnection(dcsConnection),
	m_connectOnOpen(connectOnOpen),
	m_elementName("New Connection")
{
	m_sendCallback = std::bind(&ConnectionWindow::SendCallback, this, std::placeholders::_1);
	m_recvCallback = std::bind(&ConnectionWindow::RecvCallback, this, std::placeholders::_1, std::placeholders::_2);
	m_connectCallback = std::bind(&ConnectionWindow::ConnectCallback, this, std::placeholders::_1, std::placeholders::_2);
	m_closeCallback = std::bind(&ConnectionWindow::CloseCallback, this);

	if (dcsConnection && connectOnOpen)
		m_dcsConnection->Connect(m_host.c_str(), m_port.c_str(), m_sendCallback, m_recvCallback, m_connectCallback, m_closeCallback);
}

void GUI::ConnectionWindow::Show()
{
	m_elementName = m_connection ? m_connection->GetName() : m_elementName;
	if (m_isOpened)
	{
		ImGui::Begin(m_elementName.c_str(), &m_isOpened);
			ImGui::BeginChild("##", { 0, 0 }, false, 0);

			std::string outStr;
			for (auto str : m_cmdStrings)
			{
				outStr.append(str.c_str());
				outStr.append("\n");
			}
			ImGui::PushItemWidth(-1);
			auto h = ImGui::GetWindowHeight();
			ImGui::InputTextMultiline("##", outStr.data(), strlen(outStr.data()), ImVec2(-1, 2 * h / 3), ImGuiInputTextFlags_ReadOnly);
			ImGui::PopItemWidth();

			//ImGui::ProgressBar(0.0, )
			if (ImGui::Button("Terminate"))
			{
				if (m_connection)
					m_connection->Terminate();
			}

			if (ImGui::Button("Connect"))
			{
				if (m_dcsConnection)
					m_dcsConnection->Connect(m_host.c_str(), m_port.c_str(), m_sendCallback, m_recvCallback, m_connectCallback, m_closeCallback);
			}

			ImGui::EndChild();
		ImGui::End();
	}
}

bool GUI::ConnectionWindow::IsOpened()
{
	return m_isOpened;
}

std::string GUI::ConnectionWindow::GetName()
{
	return m_elementName;
}

void GUI::ConnectionWindow::SendCallback(const Network::Connection::OperationResult& result)
{
	switch (result)
	{
	case Network::Connection::OperationResult::Failure:
		std::cout << "Send failed" << std::endl;
		break;
	case Network::Connection::OperationResult::Timeout:
		std::cout << "Send timeout" << std::endl;
		break;
	case Network::Connection::OperationResult::Success:
	default:
		break;
	}
}

void GUI::ConnectionWindow::RecvCallback(const Network::Connection::EventType& e, const std::string& message)
{
	m_cmdStrings.push_back(Format(message));
}

void GUI::ConnectionWindow::ConnectCallback(const Network::Connection::OperationResult& result, std::shared_ptr<Network::AsyncConnection> pConnection)
{
	switch (result)
	{
	case Network::Connection::OperationResult::Failure:
		m_cmdStrings.push_back(Format("Connection failure"));
		break;
	case Network::Connection::OperationResult::Timeout:
		m_cmdStrings.push_back(Format("Connection timeout"));
		break;
	case Network::Connection::OperationResult::Success:
		m_cmdStrings.push_back(Format("Connection established"));
		m_connection = pConnection;
	default:
		break;
	}
}

void GUI::ConnectionWindow::CloseCallback()
{
	m_cmdStrings.push_back(Format("Connection closed"));
}
