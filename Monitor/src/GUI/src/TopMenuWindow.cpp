#include "../TopMenuWindow.h"
#include "../WindowContainer.h"
#include "../CreateConnectionWindow.h"

void GUI::TopMenuWindow::Show()
{
	if(ImGui::GetIO().KeyCtrl && ImGui::IsKeyDown(ImGuiKey_O))
		WindowContainer::GetInstance()->AddElement(std::make_shared<CreateConnectionWindow>());

	if (ImGui::BeginMainMenuBar())
	{
		
		if (ImGui::BeginMenu("File"))
		{
			
			if (ImGui::MenuItem("New connection...", "CTRL+O"))
			{
				WindowContainer::GetInstance()->AddElement(std::make_shared<CreateConnectionWindow>());
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}