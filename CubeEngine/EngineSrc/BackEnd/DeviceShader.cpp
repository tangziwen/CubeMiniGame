#include "DeviceShader.h"
namespace tzw
{
int DeviceShaderBindingInfo::getBlockMemberIndex(std::string name)
{
    for(int i = 0; i < m_member.size(); i++)
    {
        if(m_member[i].name == name)
        {
            return i;
        }

    }

    return -1;
}

}
