#include <list>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "GUI/GUI.h"
#include "GUI/TopMenuWindow.h"
#include "GUI/CreateConnectionWindow.h"
#include "DCSConnection/DCSConnection.h"

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

	auto dcsConnection = std::make_shared<DCSConnection>();
	std::list<std::shared_ptr<GUI::Element>> elements;
	auto addElement = [&elements](const std::shared_ptr<GUI::Element>& element)
	{
		if (element && std::find_if(elements.begin(), elements.end(), [&element](auto pElement) { return element->GetName() == pElement->GetName(); }) == elements.end())
			elements.push_back(element);
	};
	elements.push_back(std::make_shared<GUI::TopMenuWindow>(dcsConnection, addElement));
	elements.push_back(std::make_shared<GUI::CreateConnectionWindow>(dcsConnection, addElement));

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

		for (auto elementIt = elements.begin(); elementIt != elements.end();)
		{
			auto element = *elementIt;
			if (element->IsOpened())
			{
				element->Show();
				++elementIt;
			}
			else
			{
				elementIt = elements.erase(elementIt);
			}
		}

		window.clear(bgColor);
		ImGui::SFML::Render(window);
		window.display();
	}
    
	ImGui::SFML::Shutdown();
	return 0;
}