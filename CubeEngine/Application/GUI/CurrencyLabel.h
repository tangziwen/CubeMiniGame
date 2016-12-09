#ifndef TZWS_CURRENCYLABEL_H
#define TZWS_CURRENCYLABEL_H

#include "../EngineSrc/CubeEngine.h"
#include "../EngineSrc/Engine/EngineDef.h"
#include "../EngineSrc/2D/Sprite.h"
#include "../Currency.h"
namespace tzwS {

class CurrencyLabel : public tzw::Node
{
public:
    CurrencyLabel(Currency currency, bool isCompact, bool isLarge);
    void syncData(Currency currency);
protected:
    bool m_isCompact;
    bool m_isLarge;
};

} // namespace tzwS

#endif // TZWS_CURRENCYLABEL_H
