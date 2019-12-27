#pragma once

#include "../../Interface/Drawable3D.h"
#include "../../Mesh/Mesh.h"

namespace tzw {

class LaserPrimitive : public Drawable3D
{
public:
	LaserPrimitive(vec3 begin, vec3 end, float width, bool isOriginInStart);
	virtual void submitDrawCmd(RenderCommand::RenderType passType);
	void initBuffer();
private:
	void init();
	Mesh * m_mesh;
	vec3 m_begin;
	vec3 m_end;
	float m_width;
	bool m_isOriginInStart;
};

} // namespace tzw
