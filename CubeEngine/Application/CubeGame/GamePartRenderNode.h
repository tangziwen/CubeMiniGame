#pragma once
#include "Interface/Drawable3D.h"
#include "GameItem.h"
#include "GamePartRenderMgr.h"
namespace tzw
{
class PartSurface;

class GamePartRenderNode : public Drawable3D
	{
	public:
		enum class RenderMode
		{
			COMMON,
			HIGHLIGHT,
		};
		GamePartRenderNode(VisualInfo visualInfo);
		void getCommandForInstanced(std::vector<InstanceRendereData> & commandList) override;
		void setColor(vec4 newColor) override;
		void submitDrawCmd(RenderCommand::RenderType passType);
		VisualInfo * getVisualInfo();
		PartSurface* getPartSurface() const;
		void setPartSurface(PartSurface* const partSurface);
		RenderMode getRenderMode();
		void setRenderMode(RenderMode mode);
	private:
		void updateRenderMode();
		RenderMode m_renderMode;
		VisualInfo m_visualInfo;
		std::vector<GamePartRenderInfo>m_infoList;
		PartSurface* m_partSurface;
		vec4 m_color;
		bool m_isNeedUpdateRenderInfo;
		
	};

}
