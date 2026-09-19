#pragma once
#include <deque>
#include "Math/vec2.h"
#include <string>
#include <vector>
#include <functional>
#include "Rendering/RenderFlag.h"
#include "Technique/MaterialTechnique.h"
#include "Rendering/VertexLayout.h"
namespace tzw
{
class MaterialInstance;
class ShadingParams;
class DeviceRenderPass;
class DeviceDescriptor;
class DeviceShaderCollection;
const uint32_t PIPELINE_DYNAMIC_STATE_FLAG_NONE = 0;
const uint32_t PIPELINE_DYNAMIC_STATE_FLAG_VIEWPORT = 1 << 1;
const uint32_t PIPELINE_DYNAMIC_STATE_FLAG_SCISSOR = 1 << 2;

class DevicePipeline
{
public:
	virtual ~DevicePipeline() = default;

	DevicePipeline(): m_dynamicState(PIPELINE_DYNAMIC_STATE_FLAG_NONE)
	{
	}
	
	virtual void initCompute(DeviceShaderCollection * computeShader) = 0;
	virtual void init(vec2 viewPortSize, MaterialInstance * mat, DeviceRenderPass* targetRenderPass
	                  ,VertexLayout vertexInput, bool isSupportInstancing, VertexLayout instanceVertexInput, int colorAttachmentCount = 1,
	                  MaterialTechniqueType techniqueType = MaterialTechniqueType::Default) = 0;
	virtual void resetItemWiseDescritporSet() = 0;
	virtual DeviceDescriptor * giveItemWiseDescriptorSet() = 0;
	void setDynamicState(uint32_t state)
	{
		m_dynamicState = state;
	}
	MaterialInstance * getMat()
	{
		return m_mat;
	}
	ShadingParams * getShadingParams()
	{
		return m_shadingParams;
	}
	void setShadingParams(ShadingParams * params){ m_shadingParams = params;}
protected:
	uint32_t m_dynamicState = 0;
	MaterialInstance * m_mat = nullptr;
	ShadingParams * m_shadingParams = nullptr;

};


};

