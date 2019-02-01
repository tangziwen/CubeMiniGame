#include "Island.h"
#include "../../EngineSrc/Scene/SceneMgr.h"
namespace tzw
{
	Island::Island(vec3 pos)
	{
		//the toehold here
		m_node = new Drawable3D();
		m_node->setPos(pos);
		g_GetCurrScene()->addNode(m_node);
		m_rigid = nullptr;
	}

	void Island::insert(GamePart * part)
	{
		m_partList.push_back(part);
		part->m_parent = this;
	}

}