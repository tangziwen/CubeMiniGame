#pragma once
#include <string>

namespace tzw
{
	class PartSurface
	{
	public:
		PartSurface(const std::string& diffusePath, const std::string& roughnessPath, const std::string& metallicPath,
					const std::string& normalMapPath);
		std::string getDiffusePath() const;
		void setDiffusePath(const std::string& diffusePath);
		std::string getRoughnessPath() const;
		void setRoughnessPath(const std::string& roughnessPath);
		std::string getMetallicPath() const;
		void setMetallicPath(const std::string& metallicPath);
		std::string getNormalMapPath() const;
		void setNormalMapPath(const std::string& normalMapPath);
	private:
		std::string m_diffusePath;
		std::string m_RoughnessPath;
		std::string m_metallicPath;
		std::string m_normalMapPath;
		std::string m_name;
	public:
		std::string getName() const;
		void setName(const std::string& name);
	};
}
