#include "GUI/WindowContainer.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

int main(int argc, char ** argv)
{
	sf::RenderWindow window(sf::VideoMode(960, 600), "");
	window.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(window);

	sf::Color bgColor;
	float color[3] = { 0.f, 0.f, 0.f };

	std::string windowTitle = "Monitor";
	window.setTitle(windowTitle);

	ImGui::GetIO().WantCaptureKeyboard = true;

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

		ImGui::GetIO().KeysData[ImGui::GetKeyIndex(ImGuiKey_O)].Down = sf::Keyboard::isKeyPressed(sf::Keyboard::O);

		ImGui::SFML::Update(window, deltaClock.restart());

		GUI::WindowContainer::GetInstance()->Show();

		window.clear(bgColor);
		ImGui::SFML::Render(window);
		window.display();
	}
    
	ImGui::SFML::Shutdown();
	return 0;
}