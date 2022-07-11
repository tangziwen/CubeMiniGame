#pragma once
#include "Math/vec2.h"
namespace tzw
{

struct QuadTree2DNode
{
	vec2 m_pos;
	vec2 m_size;
	QuadTree2DNode(vec2 pos, vec2 size);
	void subDivide(int & depthIndex);
};
class QuadTree2D
{

protected:
	QuadTree2DNode * m_rootNode;
};
}