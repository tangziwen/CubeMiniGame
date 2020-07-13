#include "GamePartRenderNode.h"



#include "3D/Model/Model.h"
#include "3D/Primitive/CubePrimitive.h"
#include "Rendering/Renderer.h"

namespace tzw
{
	GamePartRenderNode::GamePartRenderNode(VisualInfo visualInfo)
	{
		m_isNeedUpdateRenderInfo = true;
		m_visualInfo = visualInfo;
		setRenderMode(RenderMode::COMMON);
		auto size = m_visualInfo.size;
		//auto cube = new CubePrimitive(size.x, size.y, size.z, false);
		//m_localAABB = cube->getMesh()->getAabb();
		m_localAABB = GamePartRenderMgr::shared()->getPartLocalAABB(m_visualInfo);
	}

	void GamePartRenderNode::getCommandForInstanced(std::vector<InstanceRendereData> & commandList)
	{
		if(!m_isVisible) return;
		if(m_infoList.empty() || m_isNeedUpdateRenderInfo)
		{
			m_infoList.clear();
			m_isNeedUpdateRenderInfo = false;
			GamePartRenderMgr::shared()->getRenderInfo(true, m_visualInfo, m_partSurface, m_infoList);
		}
		for(auto info : m_infoList)
		{
		
			InstanceData instance;
			vec3 normal = vec3(0, 1,0);
			auto mat = getTransform();
			instance.posAndScale = vec4(mat.getTranslation(), 1.0);
			Quaternion q;
			getTransform().getRotation(&q);
			instance.rotateInfo = vec4(q.x, q.y, q.z, q.w);
			instance.extraInfo = m_color;
			//instance.extraInfo = m_color;
			auto data = InstanceRendereData();
			data.m_mesh = info.mesh;
			data.material = info.material;
			data.data = instance;
			commandList.push_back(data);
		}
	}

	void GamePartRenderNode::setColor(vec4 newColor)
	{
		m_color = newColor;
	}

	void GamePartRenderNode::submitDrawCmd(RenderCommand::RenderType passType)
	{
		if(getIsVisible())
		{

			if(m_infoList.empty())
			{
				GamePartRenderMgr::shared()->getRenderInfo(false, m_visualInfo, m_partSurface, m_infoList);
			}
			for(auto &info : m_infoList)
			{
				RenderCommand command(info.mesh, info.material,passType);
				setUpCommand(command);
				if(m_renderMode == RenderMode::HIGHLIGHT)
				{
					command.setRenderState(RenderFlag::RenderStage::AFTER_DEPTH_CLEAR);
				}
				Renderer::shared()->addRenderCommand(command);
			}
		}
	}

	VisualInfo* GamePartRenderNode::getVisualInfo()
	{
		return &m_visualInfo;
	}

	PartSurface* GamePartRenderNode::getPartSurface() const
	{
		return m_partSurface;
	}

	void GamePartRenderNode::setPartSurface(PartSurface* const partSurface)
	{
		m_partSurface = partSurface;
		m_isNeedUpdateRenderInfo = true;
	}
	GamePartRenderNode::RenderMode GamePartRenderNode::getRenderMode()
	{
		return m_renderMode;
	}
	void GamePartRenderNode::setRenderMode(RenderMode mode)
	{
		m_renderMode = mode;
		updateRenderMode();
	}
	void GamePartRenderNode::updateRenderMode()
	{
		switch(m_renderMode)
		{
		case RenderMode::COMMON:
			setDrawableFlag(static_cast<uint32_t>(DrawableFlag::Instancing));
			break;
		case RenderMode::HIGHLIGHT:
			setDrawableFlag(static_cast<uint32_t>(DrawableFlag::Drawable));
			break;
		}
		
	}
}

