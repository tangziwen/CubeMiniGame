#include "CurrencyLabelFactory.h"
#include "EngineSrc/Engine/Engine.h"
#include "EngineSrc/2D/LabelNew.h"
#include <stdio.h>
using namespace tzw;
namespace tzwS {
TZW_SINGLETON_IMPL(CurrencyLabelFactory)
CurrencyLabelFactory::CurrencyLabelFactory()
{

}

Node *CurrencyLabelFactory::create(Currency currency, bool isCompact, bool isLarge)
{
    char tmp[50];
    auto node = Node::create();
    if(currency.money != 0 || !isCompact)
    {
        auto subNode = Node::create();
        subNode->setName("money");
        auto sprite = Sprite::create(Engine::shared()->getUserPath("Strategy/icon/money.png"));
        subNode->addChild(sprite);
        sprintf(tmp,"%d",currency.money);
        auto label = LabelNew::create(tmp);
        subNode->addChild(label);
        node->addChild(subNode);
    }
    return node;
}

} // namespace tzwS
