#include "CurrencyLabel.h"
using namespace tzw;
namespace tzwS {

CurrencyLabel::CurrencyLabel(Currency currency, bool isCompact, bool isLarge)
{
    m_isCompact = isCompact;
    m_isLarge = isLarge;
    unsigned int advanceX = 15;
    char tmp[50];
    for(int i =0;i<=4;i++)
    {
        if(true)
        {
            auto subNode = Node::create();
            subNode->setName(currency.fieldToStr(i));
            subNode->setPos2D(advanceX,0);
            sprintf(tmp,"Strategy/icon/%s.png",currency.fieldToStr(i).c_str());
            auto sprite = Sprite::create(Engine::shared()->getUserPath(tmp));
            if(!isLarge)
            {
                sprite->setContentSize(vec2(24,24));
            }
            sprite->setName("sprite");
            sprite->setAnchorPoint(vec2(0.5,0));
            sprite->setPos2D(0,10);
            advanceX+= sprite->getContentSize().x;
            subNode->addChild(sprite);
            sprintf(tmp,"%d",currency[i]);
            auto label = LabelNew::create(tmp);
            label->setName("label");
            label->setAnchorPoint(vec2(0.5,0.0));
            label->setPos2D(0,0);
            advanceX+=label->getContentSize().x;
            subNode->addChild(label);
            addChild(subNode);
        }
    }
}

void CurrencyLabel::syncData(Currency currency)
{
    unsigned int advanceX = 15;
    char tmp[50];
    for(int i =0;i<=4;i++)
    {
        auto subNode = getChildByName(currency.fieldToStr(i));
        if(!m_isCompact || (m_isCompact && currency[i]!=0))
        {
            subNode->setIsDrawable(true);
            subNode->setPos2D(advanceX,0);
            sprintf(tmp,"Strategy/icon/%s.png",currency.fieldToStr(i).c_str());
            auto sprite = static_cast<Sprite*>(subNode->getChildByName("sprite"));
            auto widthSprite = sprite->getContentSize().x;
            sprintf(tmp,"%d",currency[i]);
            auto label = static_cast<LabelNew*>(subNode->getChildByName("label"));
            label->setString(tmp);
            auto widthLabel =label->getContentSize().x;
            advanceX += std::max(widthLabel,widthSprite) + 5;
        }else
        {
            subNode->setIsDrawable(false);
        }
    }
}
} // namespace tzwS
