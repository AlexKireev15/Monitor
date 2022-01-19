#pragma once

#include "GUI.h"

namespace GUI
{
	class TopMenuWindow : public Element
	{
	public:
		void Show() override;
		bool IsOpened() override { return true; }
		std::string GetName() override { return "Top Menu"; }
	};
}