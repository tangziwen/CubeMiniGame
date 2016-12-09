#include "PopUpMenu.h"
using namespace tzw;
namespace tzwS
{


	PopUpMenu::PopUpMenu()
	{
		m_frameBG = GUIFrame::create(vec2(100, 100));
		addChild(m_frameBG);
	}

	void PopUpMenu::insert(std::string theStr, const std::function<void (tzw::Button *)> & theCallBack)
	{
		auto btn = Button::create(theStr);
		m_btnList.push_back(btn);
		//m_frameBG->addChild(btn);
	}

	void PopUpMenu::finish()
	{
		float maxW, maxH;
		for(auto btn : m_btnList)
		{
			auto cs = btn->getContentSize();
			maxW = std::max(cs.x, maxW);
			maxH = std::max(cs.y, maxH);
		}
		float height = maxH * m_btnList.size();
		m_frameBG->setContentSize(vec2(maxW, height));
		for(int i = 0; i < m_btnList.size(); i++)
		{
			auto btn = m_btnList[i];
			btn->setContentSize(vec2(maxW, maxH));
			btn->setPos2D(0, height - (i + 1) * maxH);
			m_frameBG->addChild(btn);
		}
	}
}