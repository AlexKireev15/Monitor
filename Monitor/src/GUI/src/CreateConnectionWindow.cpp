#include "../CreateConnectionWindow.h"
#include "../WindowContainer.h"
#include "../ConnectionWindow.h"
#include "../../DCSConnection/DCSConnection.h"

void GUI::CreateConnectionWindow::Show()
{
	if (m_isOpened)
	{
		ImGui::Begin(m_elementName.c_str(), &m_isOpened);
		ImGui::BeginChild("##", { 0, 0 }, false, 0);

		if (ImGui::IsWindowAppearing())
			ImGui::SetKeyboardFocusHere();

		bool enterPressed = false;
		if (ImGui::InputText("Address", m_host, IM_ARRAYSIZE(m_host), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			if (strlen(m_host) > 0 && strlen(m_port) > 0)
				enterPressed = true;
		}
		if (ImGui::InputText("Port", m_port, IM_ARRAYSIZE(m_port), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			if (strlen(m_host) > 0 && strlen(m_port) > 0)
				enterPressed = true;
		}

		ImGui::Checkbox("Connect on open", &m_connectOnOpen);

		auto dcsConnection = GUI::WindowContainer::GetInstance()->GetDCSConnection();
		if (ImGui::Button("Connect") || enterPressed)
		{
			if (strlen(m_host) > 0 && strlen(m_port) > 0)
				GUI::WindowContainer::GetInstance()->AddElement(std::make_shared<GUI::ConnectionWindow>(m_host, m_port, dcsConnection, m_connectOnOpen));
		}

		ImGui::EndChild();
		ImGui::End();
	}
}

bool GUI::CreateConnectionWindow::IsOpened()
{
	return m_isOpened;
}

std::string GUI::CreateConnectionWindow::GetName()
{
	return m_elementName;
}
