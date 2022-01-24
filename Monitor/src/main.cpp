#include <list>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "GUI/GUI.h"
#include "GUI/TopMenuWindow.h"
#include "GUI/CreateConnectionWindow.h"
#include "DCSConnection/DCSConnection.h"

class MainLoop
{
public:
	MainLoop(const std::string & windowTitle, unsigned width, unsigned height) :
		m_window(sf::RenderWindow(sf::VideoMode(width, height), windowTitle))
	{
		m_window.setVerticalSyncEnabled(true);
		ImGui::SFML::Init(m_window);
		ImGui::GetIO().WantCaptureKeyboard = true;

		auto dcsConnection = std::make_shared<DCSConnection>();
		auto bindAddElement = std::bind(&MainLoop::AddElement, this, std::placeholders::_1);
		m_elements.push_back(std::make_shared<GUI::TopMenuWindow>(dcsConnection, bindAddElement));
		m_elements.push_back(std::make_shared<GUI::CreateConnectionWindow>(dcsConnection, bindAddElement));
	}

	~MainLoop()
	{
		ImGui::SFML::Shutdown();
	}

	void AddElement(const std::shared_ptr<GUI::Element>& element)
	{
		auto findByName = [&element](auto pElement)
		{
			return element->GetName() == pElement->GetName();
		};

		if (element && std::find_if(m_elements.begin(), m_elements.end(), findByName) == m_elements.end())
		{
			m_elements.push_back(element);
		}
	}

	void EraseClosedElements()
	{
		for (auto elementIt = m_elements.begin(); elementIt != m_elements.end();)
		{
			auto element = *elementIt;
			if (element->IsOpened())
			{
				element->Show();
				++elementIt;
			}
			else
			{
				elementIt = m_elements.erase(elementIt);
			}
		}
	}

	void Run()
	{
		sf::Clock deltaClock;
		while (m_window.isOpen())
		{
			sf::Event event;
			while (m_window.pollEvent(event))
			{
				ImGui::SFML::ProcessEvent(event);

				if (event.type == sf::Event::Closed)
				{
					m_window.close();
				}
			}

			ImGui::GetIO().KeysData[ImGui::GetKeyIndex(ImGuiKey_O)].Down = sf::Keyboard::isKeyPressed(sf::Keyboard::O);

			ImGui::SFML::Update(m_window, deltaClock.restart());
			EraseClosedElements();

			m_window.clear();
			ImGui::SFML::Render(m_window);
			m_window.display();
		}
	}


private:
	sf::RenderWindow m_window;

	std::shared_ptr<DCSConnection> m_dcsConnection;
	std::list<std::shared_ptr<GUI::Element>> m_elements;
};

int main(int argc, char ** argv)
{
	MainLoop loop("Monitor", 900u, 600u);
	loop.Run();
	return 0;
}