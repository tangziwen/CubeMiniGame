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
		void resetHistory();
		MaterialInstance* material() const;
		const DeviceAttachmentInfoList& attachments() const;
		int targetBufferIndex() const;
		int historyBufferIndex() const;
		void executeResolve(RenderGraphPassContext& graphContext, DeviceTexture* historyFrame, DeviceTexture* historyDepth, DeviceTexture* currFrame, DeviceTexture* depth);
	protected:
		static Matrix44 buildHistoryClipTransform(Matrix44 historyProjection, Matrix44 historyCameraTransform,
			Matrix44 projection, Matrix44 cameraTransform);
		MaterialInstance* m_material = nullptr;
		DeviceAttachmentInfoList m_attachments;
		Matrix44 m_historyProjection;
		Matrix44 m_historyCameraTransform;
		Camera* m_historyCamera = nullptr;
		vec2 m_historySize;
		vec2 m_historyClipPlanes;
		bool m_hasHistory = false;
		bool m_framePending = false;
		int m_index = 0;
		int m_targetBufferIndex = 0;
		// Current-frame jitter in pixels, applied to scene rendering and passed as TU_jitterUV.
		vec2 m_offset;
		// TSAA resolve tuning parameters.
		float m_jitterScalePixels = 1.0f;
		// History weight, moving history weight, depth-edge threshold, motion scale.
		vec4 m_resolveParams = vec4(0.90f, 0.10f, 0.010f, 0.15f);
		// Relative luma threshold, edge history weight, relative/absolute depth tolerance.
		vec4 m_rejectionParams = vec4(0.20f, 0.75f, 0.02f, 0.01f);
		int m_debugMode = 0;
	};
}
