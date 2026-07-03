#include "RenderSettings.h"

namespace tzw
{
bool RenderSettings::aaEnabled() const
{
	return m_aaEnabled;
}

void RenderSettings::setAAEnabled(bool enabled)
{
	m_aaEnabled = enabled;
}

bool RenderSettings::bloomEnabled() const
{
	return m_bloomEnabled;
}

void RenderSettings::setBloomEnabled(bool enabled)
{
	m_bloomEnabled = enabled;
}

bool RenderSettings::fogEnabled() const
{
	return m_fogEnabled;
}

void RenderSettings::setFogEnabled(bool enabled)
{
	m_fogEnabled = enabled;
}

bool RenderSettings::ssrEnabled() const
{
	return m_ssrEnabled;
}

void RenderSettings::setSSREnabled(bool enabled)
{
	m_ssrEnabled = enabled;
}

bool RenderSettings::ssgiEnabled() const
{
	return m_ssgiEnabled;
}

void RenderSettings::setSSGIEnabled(bool enabled)
{
	m_ssgiEnabled = enabled;
}
}
