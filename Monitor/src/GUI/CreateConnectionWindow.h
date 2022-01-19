#pragma once
#include "GUI.h"

namespace GUI
{
	class CreateConnectionWindow : public Element
	{
	public:
		void Show() override;
		bool IsOpened() override;
		std::string GetName() override;

	private:
		bool m_isOpened = true;
		char m_host[256];
		char m_port[256];
		std::string m_elementName = "Create new connection";
	};
}