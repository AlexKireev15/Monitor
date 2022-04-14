//#pragma once
//
//#include "GUI.h"
//#include "../DCSConnection/DCSConnection.h"
//
//#include <list>
//
//namespace GUI
//{
//	class TopMenuWindow : public Element
//	{
//	public:
//		TopMenuWindow(std::shared_ptr<DCSConnection> pDcsConnection,
//			std::function<void(const std::shared_ptr<Element>& element)> addElement) :
//			m_dcsConnection(pDcsConnection),
//			m_addElement(addElement) { }
//		void Show() override;
//		bool IsOpened() override { return true; }
//		std::string GetName() override { return "Top Menu"; }
//	
//	private:
//		std::shared_ptr<DCSConnection> m_dcsConnection;
//		std::function<void(const std::shared_ptr<Element>& element)> m_addElement;
//	};
//}