#include "LogicEntity.h"

namespace tzwS {

LogicEntity::LogicEntity()
{
    m_attr = new AttributeList();
}

std::string LogicEntity::name() const
{
    return m_name;
}

void LogicEntity::setName(const std::string &name)
{
    m_name = name;
}

void LogicEntity::update()
{
    printf("%s is updating",m_name.c_str());
}

AttributeList *LogicEntity::attr() const
{
    return m_attr;
}

void LogicEntity::setAttr(AttributeList *attr)
{
    m_attr = attr;
}

} // namespace tzwS
