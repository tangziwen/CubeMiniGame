#include "GUIRadioFrame.h"
#include <algorithm>

namespace tzw {

GUIRadioFrame::GUIRadioFrame(): m_tipsLabel(nullptr), m_detailLabel(nullptr)
{
	m_expandType = ExpandType::Horizontal;
	m_focusBtn = nullptr;
	m_stride = 10;
	m_btnsStride = 10;
	m_marginHorizontal = vec2(10, 10);
}

GUIRadioFrame *GUIRadioFrame::create(std::string titleText, vec4 color, vec2 size)
{
	auto frame = new GUIRadioFrame();
	frame->initLabel();
	frame->setUniformColor(color);
	frame->setTitle(titleText);
	frame->setContentSize(size);
	frame->initDetailLabel();
	frame->initTipsLabel();
	return frame;
}

tzw::GUIRadioFrame *tzw::GUIRadioFrame::create(std::string titleText, vec2 size)
{
    return GUIRadioFrame::create(titleText,vec4(53.0f/255,53.0f/255,53.0f/255,1.0f),size);
}

GUIRadioFrame *GUIRadioFrame::create(std::string titleText)
{
    return GUIRadioFrame::create(titleText,vec2(100,100));
}

void GUIRadioFrame::addRadioButton(std::string buttonName,std::function<void(Button*)> btnCallback)
{
	Button * startBtn = Button::create(buttonName);
	startBtn->setOnBtnClicked(std::bind(&GUIRadioFrame::onButtonClicked,this,std::placeholders::_1));
	this->addChild(startBtn);
	m_buttonCallbackMap[startBtn] = btnCallback;
	m_btnList.push_back(startBtn);
}

GUIRadioFrame::ExpandType GUIRadioFrame::getExpandType() const
{
	return m_expandType;
}

void GUIRadioFrame::setExpandType(const ExpandType &expandType)
{
	m_expandType = expandType;
}

void GUIRadioFrame::flush()
{
	focusAndTintColor(m_btnList.front());
	switch(m_expandType)
	{
	case ExpandType::Vertical:
		flushVertical();
		break;
	case ExpandType::Horizontal:
		flushHorizontal();
		break;
	}
}

void GUIRadioFrame::onButtonClicked(Button *btn)
{
	focusAndTintColor(btn);
	auto result = m_buttonCallbackMap.find(btn);
	if(result != m_buttonCallbackMap.end())
	{
		auto callback = m_buttonCallbackMap[btn];
		if(callback)
		{
			callback(btn);
		}
	}
}

void tzw::GUIRadioFrame::flushVertical()
{
}

void tzw::GUIRadioFrame::flushHorizontal()
{
	float advance_y = m_stride;
    float frame_width = 0;
    float frame_height = 0;
	float buttonWidth = 0;
	float buttonHeight = 0;
	for(auto btn :m_btnList)
	{
        buttonWidth += btn->getContentSize().x + m_btnsStride;
		buttonHeight = btn->getContentSize().y;
	}
    frame_width = std::max(std::max(m_detailLabel->getContentSize().x,m_tipsLabel->getContentSize().x),buttonWidth) + m_marginHorizontal.x + m_marginHorizontal.y;
    frame_height = m_tipsLabel->getContentSize().y + buttonHeight + m_tipsLabel->getContentSize().y + 4 * m_stride;
    auto preFrameHeight = getContentSize().y;
    setContentSize(vec2(frame_width,std::max(frame_height,preFrameHeight)));
    auto offsetY = preFrameHeight - frame_height;

    m_detailLabel->setPos2D(m_marginHorizontal.x,advance_y+offsetY);
    advance_y+=m_detailLabel->getContentSize().y + m_stride;
    float advance_x = m_marginHorizontal.x;
	for(auto btn:m_btnList)
	{
        btn->setPos2D(advance_x,advance_y+offsetY);
		advance_x += btn->getContentSize().x + m_btnsStride;
	}
    advance_y += buttonHeight + m_stride + m_tipsLabel->getContentSize().y;
    m_tipsLabel->setPos2D(m_marginHorizontal.x,advance_y+offsetY);
}

void GUIRadioFrame::focusAndTintColor(Button *btn)
{
	if(m_focusBtn)
	{
		m_focusBtn->getLabel()->setUniformColor(vec3(1.0,1.0,1.0));
	}
	m_focusBtn = btn;
	m_focusBtn->getLabel()->setUniformColor(vec3(1.0,0.0,0.0));
}

void GUIRadioFrame::initTipsLabel()
{
	m_tipsLabel = LabelNew::create("some tips");
	addChild(m_tipsLabel);
	auto contentSize = this->getContentSize();
	m_tipsLabel->setPos2D(0,contentSize.y - 15);
}

void GUIRadioFrame::initDetailLabel()
{
	m_detailLabel = LabelNew::create("some detail");
	addChild(m_detailLabel);
	m_detailLabel->setPos2D(0,0);
}

vec2 GUIRadioFrame::getMarginHorizontal() const
{
    return m_marginHorizontal;
}

void GUIRadioFrame::setMarginHorizontal(const vec2 &marginHorizontal)
{
    m_marginHorizontal = marginHorizontal;
}

LabelNew *GUIRadioFrame::getDetailLabel() const
{
    return m_detailLabel;
}

LabelNew *GUIRadioFrame::getTipsLabel() const
{
    return m_tipsLabel;
}

float GUIRadioFrame::getBtnsStride() const
{
	return m_btnsStride;
}

void GUIRadioFrame::setBtnsStride(float btnsStride)
{
	m_btnsStride = btnsStride;
}

float GUIRadioFrame::getStride() const
{
	return m_stride;
}

void GUIRadioFrame::setStride(float stride)
{
	m_stride = stride;
}

} // namespace tzw
