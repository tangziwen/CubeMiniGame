#include "DeviceDEscriptorVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
#include "DeviceBufferVK.h"
#include "DeviceDescriptorSetLayoutVK.h"
namespace tzw
{
    DeviceDescriptorSetLayoutVK::DeviceDescriptorSetLayoutVK(unsigned setID):
        m_setID(setID),m_layout(0)
    {
    }
    unsigned DeviceDescriptorSetLayoutVK::getSetID()
    {
        return m_setID;
    }
    bool DeviceDescriptorSetLayoutVK::isHaveThisBinding(unsigned bindingID)
    {
        return m_bindingSet.find(bindingID) != m_bindingSet.end();
    }
    unsigned DeviceDescriptorSetLayoutVK::getBindingFromName(std::string name)
    {
        return m_nameToBindingMap[name];
    }
    void DeviceDescriptorSetLayoutVK::addBinding(unsigned bindingId, std::string name)
    {
        m_nameToBindingMap[name] = bindingId;
        m_bindingSet.insert(bindingId);
    }
    void DeviceDescriptorSetLayoutVK::setLayout(VkDescriptorSetLayout layout)
    {
        m_layout = layout;
    }
    VkDescriptorSetLayout DeviceDescriptorSetLayoutVK::getLayout()
    {
        return m_layout;
    }
}