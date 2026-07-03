#pragma once

namespace tzw
{
class RenderSettings
{
public:
	bool aaEnabled() const;
	void setAAEnabled(bool enabled);
	bool bloomEnabled() const;
	void setBloomEnabled(bool enabled);
	bool fogEnabled() const;
	void setFogEnabled(bool enabled);
	bool ssrEnabled() const;
	void setSSREnabled(bool enabled);
	bool ssgiEnabled() const;
	void setSSGIEnabled(bool enabled);

private:
	bool m_aaEnabled = true;
	bool m_bloomEnabled = true;
	bool m_fogEnabled = true;
	bool m_ssrEnabled = true;
	bool m_ssgiEnabled = true;
};
}
