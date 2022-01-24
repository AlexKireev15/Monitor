#pragma once

#include "imgui.h"
#include "imgui-sfml.h"

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