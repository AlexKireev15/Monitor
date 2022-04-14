//#include "..\ConnectionWindow.h"
//
//std::string GetTimestamp()
//{
//	SYSTEMTIME timestamp;
//	GetLocalTime(&timestamp);
//	auto resultedString = std::to_string(timestamp.wHour) + ":" +
//		std::to_string(timestamp.wMinute) + ":" +
//		std::to_string(timestamp.wSecond) + ":" +
//		std::to_string(timestamp.wMilliseconds);
//	for (size_t idx = resultedString.length(); idx < 13u; ++idx)
//	{
//		resultedString += " ";
//	}
//	return resultedString;
//}
//
//std::string Format(std::string str)
//{
//	str.erase(std::remove(str.begin(), str.end(), '\0'), str.end());
//	return GetTimestamp() + " " + str + "\n";
//}
//
//GUI::ConnectionWindow::ConnectionWindow(const std::string& host, const std::string& port, std::shared_ptr<DCSConnection> dcsConnection, bool connectOnOpen) :
//	m_host(host), m_port(port),
//	m_dcsConnection(dcsConnection),
//	m_connectOnOpen(connectOnOpen),
//	m_elementName("New Connection")
//{
//	m_sendCallback = std::bind(&ConnectionWindow::SendCallback, this, std::placeholders::_1);
//	m_recvCallback = std::bind(&ConnectionWindow::RecvCallback, this, std::placeholders::_1, std::placeholders::_2);
//	m_connectCallback = std::bind(&ConnectionWindow::ConnectCallback, this, std::placeholders::_1, std::placeholders::_2);
//	m_closeCallback = std::bind(&ConnectionWindow::CloseCallback, this);
//
//	if (dcsConnection && connectOnOpen)
//		m_dcsConnection->Connect(m_host.c_str(), m_port.c_str(), this, m_sendCallback, m_recvCallback, m_connectCallback, m_closeCallback);
//
//	m_elementName = m_connection ? m_connection->GetName() : m_elementName;
//	ImGui::SetNextWindowSize({ 400, 400 });
//}
//
//void GUI::ConnectionWindow::Show()
//{
//	ImGui::Begin(m_elementName.c_str(), &m_isOpened);
//		if (!m_isOpened)
//		{
//			Close();
//			ImGui::End();
//			return;
//		}
//		ImGui::BeginChild("##", { 0, 0 }, false, 0);
//
//		ImGui::PushItemWidth(-1);
//		auto h = ImGui::GetWindowHeight();
//		auto cmdInputTextLabel = "CmdInputTextMultiline";
//
//		m_cmdStringsMutex.lock();
//		ImGui::InputTextMultiline(cmdInputTextLabel, m_cmdStrings.data(), strlen(m_cmdStrings.data()), ImVec2(-1, 2 * h / 3), ImGuiInputTextFlags_ReadOnly);
//		m_cmdStringsMutex.unlock();
//
//		ImGui::BeginChild(cmdInputTextLabel);
//		if (ImGui::GetIO().MouseWheel > 0)
//			m_isAutoScroll = false;
//		if (abs(ImGui::GetScrollMaxY() - ImGui::GetScrollY())  < 1e-10)
//			m_isAutoScroll = true;
//		if(m_isAutoScroll)
//			ImGui::SetScrollHereY(1.0f);
//		ImGui::EndChild();
//
//		ImGui::PopItemWidth();
//
//		if (!m_connection)
//		{
//			if (ImGui::Button("Connect"))
//			{
//				if (m_dcsConnection)
//					m_dcsConnection->Connect(m_host.c_str(), m_port.c_str(), this, m_sendCallback, m_recvCallback, m_connectCallback, m_closeCallback);
//			}
//		}
//
//		if (ImGui::Button("Close"))
//		{
//			m_isOpened = false;
//			Close();
//			ImGui::EndChild();
//			ImGui::End();
//			return;
//		}
//		if (ImGui::Button("Terminate process"))
//		{
//			if (m_connection)
//				m_connection->Exit();
//		}
//		ImGui::EndChild();
//	ImGui::End();
//}
//
//bool GUI::ConnectionWindow::IsOpened()
//{
//	return m_isOpened;
//}
//
//std::string GUI::ConnectionWindow::GetName()
//{
//	return m_elementName;
//}
//
//void GUI::ConnectionWindow::Close()
//{
//	if(m_dcsConnection)
//		m_dcsConnection->Exit(this, m_connection);
//	m_connection.reset();
//}
//
//void GUI::ConnectionWindow::SendCallback(const Network::Connection::OperationResult& result)
//{
//	switch (result)
//	{
//	case Network::Connection::OperationResult::Failure:
//		std::cout << "Send failed" << std::endl;
//		break;
//	case Network::Connection::OperationResult::Timeout:
//		std::cout << "Send timeout" << std::endl;
//		break;
//	case Network::Connection::OperationResult::Success:
//	default:
//		break;
//	}
//}
//
//void GUI::ConnectionWindow::RecvCallback(const Network::Connection::EventType& e, const std::string& message)
//{
//	std::unique_lock<std::mutex> locker(m_cmdStringsMutex);
//	m_cmdStrings += Format(message);
//}
//
//void GUI::ConnectionWindow::ConnectCallback(const Network::Connection::OperationResult& result, std::shared_ptr<Network::AsyncConnection> pConnection)
//{
//	std::unique_lock<std::mutex> locker(m_cmdStringsMutex);
//	switch (result)
//	{
//	case Network::Connection::OperationResult::Failure:
//		m_cmdStrings += Format("Connection failure");
//		break;
//	case Network::Connection::OperationResult::Timeout:
//		m_cmdStrings += Format("Connection timeout");
//		break;
//	case Network::Connection::OperationResult::Success:
//		m_cmdStrings += Format("Connection established");
//		m_connection = pConnection;
//	default:
//		break;
//	}
//}
//
//void GUI::ConnectionWindow::CloseCallback()
//{
//	std::unique_lock<std::mutex> locker(m_cmdStringsMutex);
//	m_cmdStrings += Format("Connection closed");
//
//}
