#include <list>

#include "GUI/GUI.h"
//#include "GUI/TopMenuWindow.h"
//#include "GUI/CreateConnectionWindow.h"
//#include "DCSConnection/DCSConnection.h"

//class MainLoop
//{
//public:
//	MainLoop(const std::string & windowTitle, unsigned width, unsigned height) :
//		m_window(sf::RenderWindow(sf::VideoMode(width, height), windowTitle))
//	{
//		m_window.setVerticalSyncEnabled(true);
//		ImGui::SFML::Init(m_window);
//		ImGui::GetIO().WantCaptureKeyboard = true;
//
//		auto dcsConnection = std::make_shared<DCSConnection>();
//		auto bindAddElement = std::bind(&MainLoop::AddElement, this, std::placeholders::_1);
//		m_elements.push_back(std::make_shared<GUI::TopMenuWindow>(dcsConnection, bindAddElement));
//		m_elements.push_back(std::make_shared<GUI::CreateConnectionWindow>(dcsConnection, bindAddElement));
//	}
//
//	~MainLoop()
//	{
//		ImGui::SFML::Shutdown();
//	}
//
//	void AddElement(const std::shared_ptr<GUI::Element>& element)
//	{
//		auto findByName = [&element](auto pElement)
//		{
//			return element->GetName() == pElement->GetName();
//		};
//
//		if (element && std::find_if(m_elements.begin(), m_elements.end(), findByName) == m_elements.end())
//		{
//			m_elements.push_back(element);
//		}
//	}
//
//	void EraseClosedElements()
//	{
//		for (auto elementIt = m_elements.begin(); elementIt != m_elements.end();)
//		{
//			auto element = *elementIt;
//			if (element->IsOpened())
//			{
//				element->Show();
//				++elementIt;
//			}
//			else
//			{
//				elementIt = m_elements.erase(elementIt);
//			}
//		}
//	}
//
//	void Run()
//	{
//		sf::Clock deltaClock;
//		while (m_window.isOpen())
//		{
//			sf::Event event;
//			while (m_window.pollEvent(event))
//			{
//				ImGui::SFML::ProcessEvent(event);
//
//				if (event.type == sf::Event::Closed)
//				{
//					m_window.close();
//				}
//			}
//
//			ImGui::GetIO().KeysData[ImGui::GetKeyIndex(ImGuiKey_O)].Down = sf::Keyboard::isKeyPressed(sf::Keyboard::O);
//
//			ImGui::SFML::Update(m_window, deltaClock.restart());
//			EraseClosedElements();
//
//			m_window.clear();
//			ImGui::SFML::Render(m_window);
//			m_window.display();
//		}
//	}
//
//
//private:
//	sf::RenderWindow m_window;
//
//	std::shared_ptr<DCSConnection> m_dcsConnection;
//	std::list<std::shared_ptr<GUI::Element>> m_elements;
//};

int main(int argc, char ** argv)
{
	//MainLoop loop("Monitor", 900u, 600u);
	//loop.Run();
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;
    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        //Create window
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get window surface
            screenSurface = SDL_GetWindowSurface(window);

            //Fill the surface white
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

            //Update the surface
            SDL_UpdateWindowSurface(window);

            //Wait two seconds
            SDL_Delay(2000);
        }
    }
    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}