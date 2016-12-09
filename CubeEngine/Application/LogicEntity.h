#ifndef TZWS_LOGICENTITY_H
#define TZWS_LOGICENTITY_H

#include <string>
#include "AttributeList.h"
namespace tzwS {

class LogicEntity
{
public:
    LogicEntity();
    std::string name() const;
    void setName(const std::string &name);
    virtual void update();
    AttributeList *attr() const;
    void setAttr(AttributeList *attr);

protected:
    std::string m_name;
    AttributeList * m_attr;
};

} // namespace tzwS

#endif // TZWS_LOGICENTITY_H
