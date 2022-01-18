#pragma once

#include "imgui.h"
#include "imgui-sfml.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "../DCSConnection/DCSConnection.h"

#include <sysinfoapi.h>

namespace GUI 
{
	class Element
	{
	public:
		virtual void Show() = 0;
		~Element() { }
	};
	void ShowConnectionWindow(const std::string& host, const std::string& port);
}