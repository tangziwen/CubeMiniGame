#include "DebugPrimitive.h"
#include "3D/Primitive/LinePrimitive.h"
#include "Rendering/RenderFlag.h"

namespace tzw
{

DebugPrimitive::DebugPrimitive()
	:m_line(new LinePrimitive()), m_defaultColor(1, 0, 0), m_isVisible(true), m_destroyRequested(false)
{
}

DebugPrimitive::~DebugPrimitive()
{
}

void DebugPrimitive::setVisible(bool visible)
{
	m_isVisible = visible;
}

bool DebugPrimitive::isVisible() const
{
	return m_isVisible;
}

void DebugPrimitive::setDefaultColor(vec3 color)
{
	m_defaultColor = color;
}

vec3 DebugPrimitive::getDefaultColor() const
{
	return m_defaultColor;
}

void DebugPrimitive::setColor(vec3 color)
{
	m_defaultColor = color;
	m_line->setColor(color);
}

void DebugPrimitive::requestDestroy()
{
	m_destroyRequested = true;
}

bool DebugPrimitive::isDestroyRequested() const
{
	return m_destroyRequested;
}

void DebugPrimitive::clear()
{
	m_line->clear();
}

void DebugPrimitive::appendLine(vec3 begin, vec3 end)
{
	m_line->append(begin, end, m_defaultColor);
}

void DebugPrimitive::appendLine(vec3 begin, vec3 end, vec3 color)
{
	m_line->append(begin, end, color);
}

int DebugPrimitive::getLineCount() const
{
	return m_line->getLineCount();
}

void DebugPrimitive::submitDrawCmd(RenderQueue* queue, int requirementArg)
{
	if(!m_isVisible || m_line->getLineCount() <= 0)
	{
		return;
	}
	m_line->initBuffer();
	m_line->submitDrawCmd(RenderFlag::RenderStage::COMMON, queue, requirementArg);
}

LinePrimitive* DebugPrimitive::getLinePrimitive()
{
	return m_line.get();
}

} // namespace tzw
