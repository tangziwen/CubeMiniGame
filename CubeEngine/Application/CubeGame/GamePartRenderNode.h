#pragma once
#include "Interface/Drawable3D.h"
#include "GameItem.h"
#include "GamePartRenderMgr.h"
namespace tzw
{
class PartSurface;
class GamePart;
class GamePartRenderNode : public Drawable3D
	{
	public:
		enum class RenderMode
		{
			COMMON,
			AFTER_DEPTH,
			NO_INSTANCING,
		};
		GamePartRenderNode(GameItem * item, GamePart * partInstance);
		void getInstancedData(std::vector<InstanceRendereData> & commandList) override;
		void setColor(vec4 newColor) override;
		void submitDrawCmd(RenderFlag::RenderStage requirementType, RenderQueues * queues, int requirementArg) override;
		VisualInfo * getVisualInfo();
		PartSurface* getPartSurface() const;
		void setPartSurface(PartSurface* const partSurface);
		RenderMode getRenderMode();
		void setRenderMode(RenderMode mode);
		void setSpecifiedMat(Material * mat);
		Material * getSpecifiedMat();
		GamePart * getPartParent();
		void forceUpdate();
		std::string getState();
		void setState(std::string newState);
		bool getIsHovering();
		void setIsHovering(bool hovering);
	private:
		std::string m_state;
		void updateRenderMode();
		GameItem * m_item;
		RenderMode m_renderMode;
		VisualInfo m_visualInfo;
		std::vector<GamePartRenderInfo>m_infoList;
		PartSurface* m_partSurface{};
		vec4 m_color;
		bool m_isNeedUpdateRenderInfo;
		Material * m_specifiedMat{};
		GamePart * m_partParent;
		bool m_isHovering;
		
	};

}
