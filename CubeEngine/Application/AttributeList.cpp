#include "AttributeList.h"

namespace tzwS {

AttributeList::AttributeList()
{

}

float AttributeList::get(std::string theName)
{
    return m_attrList[theName];
}

void AttributeList::set(std::string theName, float theValue)
{
    m_attrList[theName] = theValue;
}

} // namespace tzwS
