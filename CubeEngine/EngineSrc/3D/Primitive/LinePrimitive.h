#ifndef TZW_LINEPRIMITIVE_H
#define TZW_LINEPRIMITIVE_H
#include "../../Interface/Drawable3D.h"
#include "../../Mesh/Mesh.h"

namespace tzw {
struct LineInfo
{
	vec3 begin;
	vec3 end;
	vec3 color;
};

class LinePrimitive : public Drawable3D
{
public:
	LinePrimitive(vec3 begin, vec3 end);
	LinePrimitive();
	virtual void submitDrawCmd(RenderCommand::RenderType passType);
	void initBuffer();
	virtual void setUpTransFormation(TransformationInfo &info);
	void append(vec3 begin, vec3 end, vec3 color = vec3(1, 1, 1));
	void clear();
	int getLineCount();
private:
	void init();
	std::vector<LineInfo> m_segList;
	Mesh * m_mesh;
	vec3 m_begin;
	vec3 m_end;
};

} // namespace tzw

#endif // TZW_LINEPRIMITIVE_H
