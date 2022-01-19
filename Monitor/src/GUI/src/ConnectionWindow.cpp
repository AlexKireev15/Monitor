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
	m_host(host), m_port(m_port),
	m_dcsConnection(dcsConnection),
	m_connectOnOpen(connectOnOpen)
{
	if (dcsConnection && connectOnOpen)
		m_dcsConnection->Connect(m_host.c_str(), m_port.c_str(), SendCallback, RecvCallback, ConnectCallback, CloseCallback);
}

void GUI::ConnectionWindow::Show()
{
	sf::RenderWindow window(sf::VideoMode(960, 600), "");
	window.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(window);

	sf::Color bgColor;
	float color[3] = { 0.f, 0.f, 0.f };

	std::string windowTitle = "Monitor";
	window.setTitle(windowTitle);

	sf::Clock deltaClock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("Sample window");

		ImGui::BeginChild("", { 0, 0 }, false, 0);

		std::string outStr;
		for (auto str : m_cmdStrings)
		{
			outStr.append(str.c_str());
			outStr.append("\n");
		}
		ImGui::PushItemWidth(-1);
		auto h = ImGui::GetWindowHeight();
		ImGui::InputTextMultiline("##label", outStr.data(), strlen(outStr.data()), ImVec2(-1, 2 * h / 3), ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		//ImGui::ProgressBar(0.0, )
		if (ImGui::Button("Terminate"))
		{
			if (m_connection)
				m_connection->Terminate();
		}

		if (ImGui::Button("Connect"))
		{
			if(m_dcsConnection)
				m_dcsConnection->Connect(m_host.c_str(), m_port.c_str(), sendCallback, recvCallback, connectCallback, closeCallback);
		}

		ImGui::EndChild();

		ImGui::End();

		window.clear(bgColor);
		ImGui::SFML::Render(window);
		window.display();
	}
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
