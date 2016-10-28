#include "ConsolePanel.h"
#include "../Engine/Engine.h"
#include "../Event/EventMgr.h"
#include "../Font/FontMgr.h"
#include "../Action/MoveTo.h"
#include "../External/TUtility/TUtility.h"
#include "../Engine/EngineDef.h"
#include <strstream>
#include <iostream>
namespace tzw {

ConsolePanel::ConsolePanel(Node *renderNode)
	:m_isVisible(false),m_verticalSpace(6),m_margin(vec2(15,15)),m_heightRatio(0.85f),m_inputFrameHeight(30)
{
	m_node = new Node();
	auto winSize =  Engine::shared()->winSize();
	vec2 totalSize =vec2(winSize.x,winSize.y * m_heightRatio);


	m_bgFrame = Sprite::createWithColor(vec4(30.0/255.0,30.0/255.0,39.0/255.0,0.7),vec2(totalSize.x,totalSize.y - m_inputFrameHeight));
	m_bgFrame->setPos2D(0,m_inputFrameHeight);
	m_node->addChild(m_bgFrame);

	m_inputFrame = Sprite::createWithColor(vec4(50.0/255.0,30.0/255.0,39.0/255.0,0.7),vec2(winSize.x,m_inputFrameHeight));
	m_inputFrame->setPos2D(0,0);
	m_node->addChild(m_inputFrame);

	m_node->setPos2D(0,(1.0f - m_heightRatio)* winSize.y);

	m_label = LabelNew::create("",FontMgr::shared()->getDefaultFont());
	m_label->setPos2D(m_margin.x,7);

	m_inputFrame->addChild(m_label);
	m_node->setLocalPiority(EngineDef::maxPiority);
	setIsSwallow(true);
	renderNode->addChild(m_node);
	setFixedPiority(100);
	EventMgr::shared()->addFixedPiorityListener(this);
	m_isVisible = false;
	setIsVisible(false);

	m_scriptVM = new ScriptVM();
	m_scriptVM->useStdLibrary();
}

bool ConsolePanel::onKeyPress(int keyCode)
{
	if(m_isVisible) {
		switch(keyCode)
		{
			//删除和退格键
			case 259:
				eraseChar();
			break;
			case 261:
				eraseChar();
			break;
				//prees enter to parse
			case 257:
				{
					parse(m_label->getString());
					m_label->setString("");
				}
			break;
			default:
			break;
		}
		return true;
	}
	return false;
}

bool ConsolePanel::onCharInput(unsigned int theChar)
{
	switch(theChar)
	{
		case '`':
			toggleVissible();
		return true;
		default:
			if(m_isVisible)
			{
				auto oldS = m_label->getString();
				std::wstring theStr;
				theStr.push_back(wchar_t(theChar));
				m_label->setString(oldS + Tmisc::WstringToString(theStr));
				return true;
			}

	}
	return false;
}

bool ConsolePanel::isVisible() const
{
	return m_isVisible;
}

void ConsolePanel::setIsVisible(bool isVisible)
{
	m_isVisible = isVisible;
	if(m_isVisible)
	{
		m_node->setIsDrawable(true);
	}else
	{
		m_node->setIsDrawable(false);
	}
}

void ConsolePanel::toggleVissible()
{
	m_isVisible = !m_isVisible;
	setIsVisible(m_isVisible);
}

void ConsolePanel::print(std::string str)
{
	//return;
	if(m_labelList.size() < maxList())
	{
		auto label = LabelNew::create(str,FontMgr::shared()->getDefaultFont());
		m_bgFrame->addChild(label);
		m_labelList.push_back(label);
	}else
	{
		LabelNew * label = m_labelList.front();
		m_labelList.pop_front();
		m_labelList.push_back(label);
		label->setString(str);
	}
	updateTexts();
}

unsigned int ConsolePanel::verticalSpace() const
{
	return m_verticalSpace;
}
///
/// \brief 设置行间距
/// \param verticalSpace 行间距，默认为4
///
void ConsolePanel::setVerticalSpace(unsigned int verticalSpace)
{
	m_verticalSpace = verticalSpace;
	updateTexts();
}

vec2 ConsolePanel::margin() const
{
	return m_margin;
}

///
/// \brief 设置边距，默认为5
/// \param margin
///
void ConsolePanel::setMargin(vec2 margin)
{
	m_margin = margin;
	updateTexts();
}

void ConsolePanel::updateTexts()
{
	auto visibleRect = m_bgFrame->getContentSize();
	int y = visibleRect.y;
	for(auto iter = m_labelList.begin(); iter!= m_labelList.end() ;iter++)
	{
		LabelNew* label = (*iter);
		label->setPos2D(m_margin.x,y - m_margin.y);
		label->reCache();
		y -= label->getContentSize().y + m_verticalSpace;
	}
}

void ConsolePanel::eraseChar()
{
	auto oldStr = m_label->getString();
	if (oldStr.size()>0)
	{
		oldStr.erase(oldStr.size() -1);
		m_label->setString(oldStr);
	}
}

void ConsolePanel::parse(std::string theStr)
{
	m_scriptVM->loadFromStr(theStr,0);
	m_scriptVM->excute(0);
}

float ConsolePanel::heightRatio() const
{
	return m_heightRatio;
}

void ConsolePanel::setHeightRatio(float heightRatio)
{
	m_heightRatio = heightRatio;
}

int ConsolePanel::maxList() const
{
	auto winSize =  Engine::shared()->winSize();
	vec2 totalSize =vec2(winSize.x,winSize.y * m_heightRatio);
	auto m_outPutHeight = totalSize.y - m_inputFrameHeight;
	m_outPutHeight -= m_margin.y;
	int theMaxList = int(m_outPutHeight /(FontMgr::shared()->getDefaultFont()->getFontSize() + m_verticalSpace)) + 1;
	return theMaxList;
}
} // namespace tzw
