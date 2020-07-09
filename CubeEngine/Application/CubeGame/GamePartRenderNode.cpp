#include "GamePartRenderNode.h"


#include "GamePartRenderMgr.h"
#include "3D/Model/Model.h"
#include "3D/Primitive/CubePrimitive.h"

namespace tzw
{
	GamePartRenderNode::GamePartRenderNode(VisualInfo visualInfo)
	{
		m_visualInfo = visualInfo;
		setDrawableFlag(static_cast<uint32_t>(DrawableFlag::Instancing));


		auto size = m_visualInfo.size;
		//auto cube = new CubePrimitive(size.x, size.y, size.z, false);
		//m_localAABB = cube->getMesh()->getAabb();
		m_localAABB = GamePartRenderMgr::shared()->getPartLocalAABB(m_visualInfo);
	}

	void GamePartRenderNode::getCommandForInstanced(std::vector<InstanceRendereData> & commandList)
	{
		if(m_visualInfo.type != VisualInfo::VisualInfoType::Mesh)
		{
			auto info = GamePartRenderMgr::shared()->getRenderInfo(m_visualInfo, m_partSurface);
			InstanceData instance;
			vec3 normal = vec3(0, 1,0);
			auto mat = getTransform();
			instance.posAndScale = vec4(mat.getTranslation(), 1.0);
			Quaternion q;
			getTransform().getRotation(&q);
			instance.rotateInfo = vec4(q.x, q.y, q.z, q.w);
			//instance.extraInfo = m_color;
			auto data = InstanceRendereData();
			data.m_mesh = info.mesh;
			data.material = info.material;
			data.data = instance;
			commandList.push_back(data);
		}else
		{
			auto model = GamePartRenderMgr::shared()->getModel(m_visualInfo);
			auto meshList = model->getMeshList();
			for(auto mesh : meshList)
			{
				Material * material = nullptr;
				if(model->getMatCount() > 1)
				{
					material = model->getMat(mesh->getMatIndex());
				}
				else
				{
					material = model->getMat(0);
				}
				
				InstanceData instance;
				vec3 normal = vec3(0, 1,0);
				auto mat = getTransform();
				instance.posAndScale = vec4(mat.getTranslation(), 1.0);
				Quaternion q;
				getTransform().getRotation(&q);
				instance.rotateInfo = vec4(q.x, q.y, q.z, q.w);
				//instance.extraInfo = m_color;
				auto data = InstanceRendereData();
				data.m_mesh = mesh;
				data.material = material;
				data.data = instance;
				commandList.push_back(data);
			}
			//InstanceData instance;
		}

	}

	void GamePartRenderNode::setColor(vec4 newColor)
	{
		m_color = newColor;
	}

	PartSurface* GamePartRenderNode::getPartSurface() const
	{
		return m_partSurface;
	}

	void GamePartRenderNode::setPartSurface(PartSurface* const partSurface)
	{
		m_partSurface = partSurface;
	}
}

