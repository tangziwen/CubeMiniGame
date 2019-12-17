#include "PartSurface.h"
#include "Texture/TextureMgr.h"

namespace tzw
{
	PartSurface::PartSurface(const std::string& diffusePath, const std::string& roughnessPath,
							const std::string& metallicPath, const std::string& normalMapPath):
		m_diffusePath(diffusePath),
		m_RoughnessPath(roughnessPath),
		m_metallicPath(metallicPath),
		m_normalMapPath(normalMapPath)
	{
		//cache
		TextureMgr::shared()->getByPath(diffusePath, true);
		TextureMgr::shared()->getByPath(roughnessPath, true);
		TextureMgr::shared()->getByPath(metallicPath, true);
		TextureMgr::shared()->getByPath(normalMapPath, true);
	}

	std::string PartSurface::getDiffusePath() const
	{
		return m_diffusePath;
	}

	void PartSurface::setDiffusePath(const std::string& diffusePath)
	{
		m_diffusePath = diffusePath;
	}

	std::string PartSurface::getRoughnessPath() const
	{
		return m_RoughnessPath;
	}

	void PartSurface::setRoughnessPath(const std::string& roughnessPath)
	{
		m_RoughnessPath = roughnessPath;
	}

	std::string PartSurface::getMetallicPath() const
	{
		return m_metallicPath;
	}

	void PartSurface::setMetallicPath(const std::string& metallicPath)
	{
		m_metallicPath = metallicPath;
	}

	std::string PartSurface::getNormalMapPath() const
	{
		return m_normalMapPath;
	}

	void PartSurface::setNormalMapPath(const std::string& normalMapPath)
	{
		m_normalMapPath = normalMapPath;
	}

	std::string PartSurface::getName() const
	{
		return m_name;
	}

	void PartSurface::setName(const std::string& name)
	{
		m_name = name;
	}
}
