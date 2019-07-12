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
		if (!part->getNode()) 
		{
			printf("shit\n");
		}
		m_partList.push_back(part);
		part->m_parent = this;
	}

	void Island::remove(GamePart * part)
	{
		auto result = std::find(m_partList.begin(), m_partList.end(), part);
		if (result != m_partList.end())
		{
			m_partList.erase(result);
		}
		part->m_parent = this;
	}

}