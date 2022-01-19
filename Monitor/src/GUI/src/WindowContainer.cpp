#include "..\WindowContainer.h"
#include "..\TopMenuWindow.h"

using namespace GUI;

std::once_flag WindowContainer::m_flag;
std::unique_ptr<WindowContainer> WindowContainer::m_instance;

void GUI::WindowContainer::Show()
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

void GUI::WindowContainer::AddElement(std::shared_ptr<Element> element)
{
	if (element && std::find_if(m_elements.begin(), m_elements.end(), [&element](auto pElement) { return element->GetName() == pElement->GetName(); }) == m_elements.end())
		m_elements.push_back(element);
}

std::shared_ptr<DCSConnection> GUI::WindowContainer::GetDCSConnection()
{
	return m_dcsConnection;
}

const std::unique_ptr<WindowContainer>& GUI::WindowContainer::GetInstance()
{
	if (!m_instance)
	{
		std::call_once(m_flag, []
		{
			Initialize();
		});
	}

	return m_instance;
}

GUI::WindowContainer::WindowContainer()
{
	m_dcsConnection = std::make_shared<DCSConnection>();
	m_elements.push_back(std::make_shared<GUI::TopMenuWindow>());
}

GUI::WindowContainer::~WindowContainer()
{
}

void GUI::WindowContainer::Initialize()
{
	m_instance = std::unique_ptr<WindowContainer>(new WindowContainer);
}
