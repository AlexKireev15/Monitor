#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
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