#ifndef TZWS_ATTRIBUTELIST_H
#define TZWS_ATTRIBUTELIST_H
#define T_GET(ENTITY,ATTR) ENTITY->attr()->get(ATTR)
#define T_GETT(ATTR) this->attr()->get(ATTR)
#define T_SET(ENTITY,ATTR,VAL) ENTITY->attr()->set(ATTR , VAL)
#define T_SETT(ATTR,VAL) this->attr()->set(ATTR , VAL)
#include <unordered_map>
namespace tzwS {

class AttributeList
{
public:
    AttributeList();
    float get(std::string theName);
    void set(std::string theName,float theValue);
private:
    std::unordered_map<std::string,float> m_attrList;
};

} // namespace tzwS

#endif // TZWS_ATTRIBUTELIST_H
