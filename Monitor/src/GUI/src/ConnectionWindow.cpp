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

	ImGui::Begin(m_elementName.c_str(), &m_isOpened);
		if (!m_isOpened)
		{
			Close();
			ImGui::End();
			return;
		}
		ImGui::BeginChild("##", { 0, 0 }, false, 0);

		ImGui::PushItemWidth(-1);
		auto h = ImGui::GetWindowHeight();
		auto cmdInputTextLabel = "CmdInputTextMultiline";
		ImGui::InputTextMultiline(cmdInputTextLabel, m_cmdStrings.GetStrings(), strlen(m_cmdStrings.GetStrings()), ImVec2(-1, 2 * h / 3), ImGuiInputTextFlags_ReadOnly);
			
		ImGui::BeginChild(cmdInputTextLabel);
		if (ImGui::GetIO().MouseWheel > 0)
			m_isAutoScroll = false;
		if (abs(ImGui::GetScrollMaxY() - ImGui::GetScrollY())  < 1e-10)
			m_isAutoScroll = true;
		if(m_isAutoScroll)
			ImGui::SetScrollHereY(1.0f);
		ImGui::EndChild();

		ImGui::PopItemWidth();

		//ImGui::ProgressBar(0.0, )
		if (ImGui::Button("Start calculation"))
		{
			if (m_connection)
				m_connection->StartCalculation();
		}
		if (ImGui::Button("Terminate calculation"))
		{
			if (m_connection)
				m_connection->TerminateCalculation();
		}
		if (ImGui::Button("Terminate process"))
		{
			if (m_connection)
				m_connection->Exit();
		}

		if (ImGui::Button("Connect"))
		{
			if (m_dcsConnection)
				m_dcsConnection->Connect(m_host.c_str(), m_port.c_str(), m_sendCallback, m_recvCallback, m_connectCallback, m_closeCallback);
		}

		ImGui::EndChild();
	ImGui::End();
}

bool GUI::ConnectionWindow::IsOpened()
{
	return m_isOpened;
}

std::string GUI::ConnectionWindow::GetName()
{
	return m_elementName;
}

void GUI::ConnectionWindow::Close()
{
	if(m_dcsConnection && m_connection)
		m_dcsConnection->Exit(m_connection);
	m_connection.reset();
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
	m_cmdStrings.AddString(Format(message).c_str());
}

void GUI::ConnectionWindow::ConnectCallback(const Network::Connection::OperationResult& result, std::shared_ptr<Network::AsyncConnection> pConnection)
{
	switch (result)
	{
	case Network::Connection::OperationResult::Failure:
		m_cmdStrings.AddString(Format("Connection failure").c_str());
		break;
	case Network::Connection::OperationResult::Timeout:
		m_cmdStrings.AddString(Format("Connection timeout").c_str());
		break;
	case Network::Connection::OperationResult::Success:
		m_cmdStrings.AddString(Format("Connection established").c_str());
		m_connection = pConnection;
	default:
		break;
	}
}

void GUI::ConnectionWindow::CloseCallback()
{
	m_cmdStrings.AddString(Format("Connection closed").c_str());
}
