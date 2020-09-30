#include "DevicePipeline.h"
namespace tzw
{
void DeviceVertexInput::addVertexAttributeDesc(DeviceVertexAttributeDescVK vertexAttributeDesc)
{
    m_attributeList.emplace_back(vertexAttributeDesc);
}

}
