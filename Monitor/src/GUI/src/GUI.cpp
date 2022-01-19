#include "..\GUI.h"

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

void GUI::ShowConnectionWindow(const std::string & host, const std::string & port, DCSConnection& dcsConnection)
{
	std::vector<std::string> cmdStrings;

	std::shared_ptr<Network::AsyncConnection> connection;
	auto sendCallback = [](const Network::Connection::OperationResult& result)
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
	};
	auto recvCallback = [&cmdStrings](const Network::Connection::EventType& e, const std::string& message)
	{
		cmdStrings.push_back(Format(message));
	};
	auto connectCallback = [&connection, &cmdStrings](const Network::Connection::OperationResult& result, std::shared_ptr<Network::AsyncConnection> pConnection)
	{
		switch (result)
		{
		case Network::Connection::OperationResult::Failure:
			cmdStrings.push_back(Format("Connection failure"));
			break;
		case Network::Connection::OperationResult::Timeout:
			cmdStrings.push_back(Format("Connection timeout"));
			break;
		case Network::Connection::OperationResult::Success:
			cmdStrings.push_back(Format("Connection established"));
			connection = pConnection;
		default:
			break;
		}
	};
	auto closeCallback = [&cmdStrings]()
	{
		cmdStrings.push_back(Format("Connection closed"));
	};

	dcsConnection.Connect(host.c_str(), port.c_str(), sendCallback, recvCallback, connectCallback, closeCallback);

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
		for (auto str : cmdStrings)
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
			if (connection)
				connection->Terminate();
		}

		if (ImGui::Button("Connect"))
		{
			dcsConnection.Connect(host.c_str(), port.c_str(), sendCallback, recvCallback, connectCallback, closeCallback);
		}

		ImGui::EndChild();

		ImGui::End();

		window.clear(bgColor);
		ImGui::SFML::Render(window);
		window.display();
	}
}
