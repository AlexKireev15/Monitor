#include "../TopMenuWindow.h"
#include "../CreateConnectionWindow.h"

void GUI::TopMenuWindow::Show()
{
	if(ImGui::GetIO().KeyCtrl && ImGui::IsKeyDown(ImGuiKey_O))
		m_addElement(std::make_shared<GUI::CreateConnectionWindow>(m_dcsConnection, m_addElement));

	if (ImGui::BeginMainMenuBar())
	{
		
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New connection...", "CTRL+O"))
			{
				m_addElement(std::make_shared<GUI::CreateConnectionWindow>(m_dcsConnection, m_addElement));
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
