#include "PartSurface.h"
#include "Texture/TextureMgr.h"
#include "Utility/log/Log.h"

namespace tzw
{
	PartSurface::PartSurface(const std::string& diffusePath, const std::string& roughnessPath,
							const std::string& metallicPath, const std::string& normalMapPath):
		m_diffusePath(diffusePath),
		m_RoughnessPath(roughnessPath),
		m_metallicPath(metallicPath),
		m_normalMapPath(normalMapPath)
	{
		
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

	void PartSurface::cache()
	{
		TextureMgr::shared()->getByPath(m_diffusePath, true);
		TextureMgr::shared()->getByPath(m_diffusePath, true);
		TextureMgr::shared()->getByPath(m_RoughnessPath, true);
		TextureMgr::shared()->getByPath(m_metallicPath, true);
		TextureMgr::shared()->getByPath(m_normalMapPath, true);
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
