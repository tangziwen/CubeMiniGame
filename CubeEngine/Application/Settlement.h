#ifndef TZWS_SETTLEMENT_H
#define TZWS_SETTLEMENT_H

#include <string>
#include "AttributeList.h"
#include "LogicEntity.h"
#include "SettlementCell.h"
#include <vector>
#include "Currency.h"
#define SETTLEMENT_CELL_SIZE 16
namespace tzwS {
class Hero;
class Settlement : public LogicEntity
{
public:
    Settlement();
    Settlement(std::string theName);
    Settlement(std::string theName, Hero * ownerHero);
    Hero *owner() const;
    void setOwner(Hero *owner);

    virtual void update();
    SettlementCell * getCellIndex(int index);
    unsigned int getCellSize();
    Currency getCurrency() const;
    void setCurrency(const Currency &currency);

private:
    Currency m_currency;
    std::vector<SettlementCell*>m_cellList;
    void init();
    Hero * m_owner;
    int m_ticks;
};

} // namespace tzwS

#endif // TZWS_SETTLEMENT_H
