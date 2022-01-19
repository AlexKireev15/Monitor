#pragma once

#include "imgui.h"
#include "imgui-sfml.h"

namespace GUI 
{
	class Element
	{
	public:
		virtual void Show() = 0;
		virtual bool IsOpened() = 0;
		virtual std::string GetName() = 0;
		~Element() { }
	};
}