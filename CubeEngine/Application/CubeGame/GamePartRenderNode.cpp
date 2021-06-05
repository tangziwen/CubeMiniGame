#include "GamePartRenderNode.h"


#include "GamePart.h"
#include "3D/Model/Model.h"
#include "3D/Primitive/CubePrimitive.h"
#include "Rendering/Renderer.h"

namespace tzw
{
	GamePartRenderNode::GamePartRenderNode(GameItem * item, GamePart * partInstance)
	{
		m_isHovering = false;
		m_state = "default";
		m_isNeedUpdateRenderInfo = true;
		m_item = item;
		m_partParent = partInstance;
		m_visualInfo = m_item->m_visualInfo;
		m_partSurface = partInstance->getPartSurface();
		setRenderMode(RenderMode::COMMON);
		
		setRenderStageFlag(uint32_t(RenderFlag::RenderStage::COMMON) | uint32_t(RenderFlag::RenderStage::SHADOW));
		auto size = m_visualInfo.size;
		//auto cube = new CubePrimitive(size.x, size.y, size.z, false);
		//m_localAABB = cube->getMesh()->getAabb();
		m_localAABB = GamePartRenderMgr::shared()->getPartLocalAABB(m_visualInfo);
	}

	void GamePartRenderNode::getInstancedData(std::vector<InstanceRendereData> & dataList)
	{
		if(!m_isVisible) return;
		if(m_infoList.empty() || m_isNeedUpdateRenderInfo)
		{
			m_infoList.clear();
			m_isNeedUpdateRenderInfo = false;
			GamePartRenderMgr::shared()->getRenderInfo(true, this, m_visualInfo, m_partSurface,  m_infoList);
		}
		for(auto info : m_infoList)
		{
		
			InstanceData instance;
			vec3 normal = vec3(0, 1,0);
			auto mat = getTransform();
			instance.transform = mat;
			instance.extraInfo = (m_isHovering?vec4(0.9, 0.8, 0.1, 1.0): m_color);
			auto data = InstanceRendereData();
			data.m_mesh = info.mesh;
			data.material = info.material;
			data.data = instance;
			dataList.push_back(data);
		}
	}

	void GamePartRenderNode::setColor(vec4 newColor)
	{
		m_color = newColor;
		m_isNeedUpdateRenderInfo;
	}

	void GamePartRenderNode::submitDrawCmd(RenderFlag::RenderStage requirementType, RenderQueues * queues, int requirementArg)
	{
		if(getIsVisible())
		{
			if(m_infoList.empty()  || m_isNeedUpdateRenderInfo)
			{
				m_infoList.clear();
				GamePartRenderMgr::shared()->getRenderInfo(false, this, m_visualInfo, m_partSurface, m_infoList);
				m_isNeedUpdateRenderInfo = false;
			}
			for(auto &info : m_infoList)
			{
				RenderCommand command(info.mesh, info.material,this, info.material->getRenderStage());
				setUpCommand(command);
				if(m_renderMode == RenderMode::AFTER_DEPTH)
				{
					command.setRenderState(RenderFlag::RenderStage::AFTER_DEPTH_CLEAR);
				}
				queues->addRenderCommand(command, requirementArg);
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
		m_isNeedUpdateRenderInfo = true;
	}
	void GamePartRenderNode::setSpecifiedMat(Material* mat)
	{
		m_specifiedMat = mat;
	}
	Material* GamePartRenderNode::getSpecifiedMat()
	{
		return m_specifiedMat;
	}
	GamePart* GamePartRenderNode::getPartParent()
	{
		return m_partParent;
	}
	void GamePartRenderNode::forceUpdate()
	{
		m_isNeedUpdateRenderInfo = true;
	}
	std::string GamePartRenderNode::getState()
	{
		return m_state;
	}
	void GamePartRenderNode::setState(std::string newState)
	{
		m_state = newState;
	}
	bool GamePartRenderNode::getIsHovering()
	{
		return m_isHovering;
	}
	void GamePartRenderNode::setIsHovering(bool hovering)
	{
		m_isHovering = hovering;
	}
	void GamePartRenderNode::updateRenderMode()
	{
		switch(m_renderMode)
		{
		case RenderMode::COMMON:
			setDrawableFlag(static_cast<uint32_t>(DrawableFlag::Instancing));
			break;
		case RenderMode::NO_INSTANCING:
			setDrawableFlag(static_cast<uint32_t>(DrawableFlag::Drawable));
			break;
		case RenderMode::AFTER_DEPTH:
			setDrawableFlag(static_cast<uint32_t>(DrawableFlag::Drawable));
			break;
		}
	}
}

