#ifndef TZWS_SETTLEMENT_H
#define TZWS_SETTLEMENT_H

#include <string>
namespace tzwS {

class Settlement
{
public:
    Settlement();
    Settlement(std::string theName);
    std::string name() const;
    void setName(const std::string &name);
private:
    std::string m_name;
};

} // namespace tzwS

#endif // TZWS_SETTLEMENT_H
