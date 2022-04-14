//#pragma once
//#include "GUI.h"
//#include "../DCSConnection/DCSConnection.h"
//
//#include <list>
//
//namespace GUI
//{
//	class CreateConnectionWindow : public Element
//	{
//	public:
//		CreateConnectionWindow(std::shared_ptr<DCSConnection> pDcsConnection,
//			std::function<void(const std::shared_ptr<Element>& element)> addElement) :
//			m_dcsConnection(pDcsConnection),
//			m_addElement(addElement) { }
//		void Show() override;
//		bool IsOpened() override;
//		std::string GetName() override;
//
//	private:
//		bool m_isOpened = true;
//		bool m_connectOnOpen = true;
//		bool m_firstTimeOpen = true;
//		char m_host[256] = "";
//		char m_port[256] = "";
//		std::string m_elementName = "Create new connection";
//
//		std::shared_ptr<DCSConnection> m_dcsConnection;
//		std::function<void(const std::shared_ptr<Element>& element)> m_addElement;
//	};
//}