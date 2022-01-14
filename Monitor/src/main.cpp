#include "imgui.h"
#include "imgui-sfml.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "DCSConnection/DCSConnection.h"

std::vector<std::string> g_strings;

int main(int argc, char ** argv)
{
	DCSConnection connection;
	connection.Connect("localhost", "27015", [](const std::string& str)
		{
            g_strings.push_back(str);
		});

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

        ImGui::BeginChild("Sample scroll", { 0, 0 }, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        for (auto str : g_strings)
        {
            ImGui::Text(str.c_str());
        }

        ImGui::EndChild();

        ImGui::End();

        window.clear(bgColor);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

	return 0;
}