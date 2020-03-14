#include "PaintGun.h"
#include "CubeGame/PartSurfaceMgr.h"
#include "Texture/TextureMgr.h"
#include "GamePart.h"
#include "GameItem.h"
namespace tzw
{
	PaintGun::PaintGun():color(vec3(1, 1,1)),m_surface(PartSurfaceMgr::shared()->getItem("Metal Grid3")),m_matIndex(0)
	{
	}

	void PaintGun::paint(GamePart* part)
	{
		part->setSurface(color, m_surface);
		//part->getNode()->setColor(vec4(color, 1.0));
		//
		//if(part->getItem()->m_visualInfo.type != VisualInfo::VisualInfoType::Mesh)
		//{
		//	auto mat = part->getNode()->getMaterial();
		//	auto texture =  TextureMgr::shared()->getByPath(m_surface->getDiffusePath());
		//	mat->setTex("DiffuseMap", texture);

		//	auto metallicTexture =  TextureMgr::shared()->getByPath(m_surface->getMetallicPath());
		//	mat->setTex("MetallicMap", metallicTexture);

		//	auto roughnessTexture =  TextureMgr::shared()->getByPath(m_surface->getRoughnessPath());
		//	mat->setTex("RoughnessMap", roughnessTexture);

		//	auto normalMapTexture =  TextureMgr::shared()->getByPath(m_surface->getNormalMapPath());
		//	mat->setTex("NormalMap", normalMapTexture);
		//}

	}
}
