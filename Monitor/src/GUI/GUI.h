#pragma once

#include "imgui.h"
#include "imgui-sfml.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <mutex>

namespace GUI 
{
	class Element
	{
	public:
		virtual void Show() = 0;
		virtual bool IsOpened() = 0;
		virtual std::string GetName() = 0;
		virtual inline std::unique_lock<std::mutex> Lock()
		{
			return std::unique_lock<std::mutex>(m_elementMutex);
		}

		~Element() { }

	protected:
		std::mutex m_elementMutex;
	};
}