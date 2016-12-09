#ifndef TZWS_CURRENCYLABELFACTORY_H
#define TZWS_CURRENCYLABELFACTORY_H
#include "../EngineSrc/CubeEngine.h"
#include "../EngineSrc/Engine/EngineDef.h"
#include "../EngineSrc/2D/Sprite.h"
#include "../Currency.h"
namespace tzwS {

class CurrencyLabelFactory
{
public:
    CurrencyLabelFactory();
    tzw::Node * create(Currency currency,bool isCompact, bool isLarge);
    TZW_SINGLETON_DECL(CurrencyLabelFactory)
};

} // namespace tzwS

#endif // TZWS_CURRENCYLABELFACTORY_H
