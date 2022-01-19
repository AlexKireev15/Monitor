#pragma once

#include <mutex>
#include "GUI.h"
#include "../DCSConnection/DCSConnection.h"

namespace GUI
{
	class WindowContainer : public Element
	{
		friend struct std::default_delete<WindowContainer>;
	public:
		void Show() override;
		bool IsOpened() override { return true; }
		std::string GetName() override { return "Window Container"; }

		void AddElement(std::shared_ptr<Element> element);
		std::shared_ptr<DCSConnection> GetDCSConnection();

		static const std::unique_ptr<WindowContainer>& GetInstance();

	protected:
		WindowContainer();
		virtual ~WindowContainer();

		static void Initialize();

	private:
		std::list<std::shared_ptr<Element>> m_elements;
		std::shared_ptr<DCSConnection> m_dcsConnection;

		static std::unique_ptr<WindowContainer> m_instance;
		static std::once_flag m_flag;
	};
};