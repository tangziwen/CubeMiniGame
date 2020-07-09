#pragma once
#include "Interface/Drawable3D.h"
#include "GameItem.h"
namespace tzw
{
class PartSurface;

class GamePartRenderNode : public Drawable3D
	{
	public:
		GamePartRenderNode(VisualInfo visualInfo);
		void getCommandForInstanced(std::vector<InstanceRendereData> & commandList) override;
		void setColor(vec4 newColor) override;
	private:
		VisualInfo m_visualInfo;
	public:
		PartSurface* getPartSurface() const;
		void setPartSurface(PartSurface* const partSurface);
	private:
		PartSurface* m_partSurface;
		vec4 m_color;
	};

}
