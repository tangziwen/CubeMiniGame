#pragma once

#include "BackEnd/DeviceRenderPass.h"
#include "Math/Matrix44.h"
#include "Math/vec2.h"
namespace tzw
{
class Camera;
	class DeviceTexture;
	class MaterialInstance;
	class RenderGraphPassContext;

	class TSAA
	{
	public:
		void init();
		void preTick(Camera* camera);
		MaterialInstance* material() const;
		const DeviceAttachmentInfoList& attachments() const;
		int targetBufferIndex() const;
		int historyBufferIndex() const;
		void executeResolve(RenderGraphPassContext& graphContext, DeviceTexture* historyFrame, DeviceTexture* currFrame, DeviceTexture* depth);
	protected:
		MaterialInstance* m_material = nullptr;
		DeviceAttachmentInfoList m_attachments;
		// Previous-frame unjittered view-projection matrix for history reprojection.
		Matrix44 m_lastViewProj;
		int m_index = 0;
		int m_targetBufferIndex = 0;
		// Current-frame jitter in pixels, applied to scene rendering and passed as TU_jitterUV.
		vec2 m_offset;
		// TSAA resolve tuning parameters.
		float m_jitterScalePixels = 1.0f;
		vec4 m_resolveParams = vec4(0.94f, 0.10f, 0.010f, 0.05f);
		vec4 m_rejectionParams = vec4(1.00f, 0.92f, 0.0f, 0.0f);
		int m_debugMode = 0;
	};
}
