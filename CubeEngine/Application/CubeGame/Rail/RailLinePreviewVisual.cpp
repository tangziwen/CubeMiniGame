#include "RailLinePreviewVisual.h"

#include "RailLineVisualStyle.h"
#include "EngineSrc/3D/Primitive/LinePrimitive.h"
#include "EngineSrc/Base/Node.h"

#include <algorithm>
#include <cmath>

namespace tzw {

namespace
{
constexpr float PreviewSampleSpacing = 0.75f;
const vec3 PreviewOffset(0.0f, 0.16f, 0.0f);
const vec3 OverviewOffset(0.0f, 0.12f, 0.0f);
const vec3 SelectedOverviewOffset(0.0f, 0.18f, 0.0f);
}

RailLinePreviewVisual::~RailLinePreviewVisual()
{
	clear();
}

void RailLinePreviewVisual::show(Node* sceneRoot, const RailNetwork& network, const RailLine* line)
{
	const vec4 color = line ? railLineSelectedColor(line->id) : vec4(0.1f, 0.85f, 0.25f, 1.0f);
	show(sceneRoot, network, line, vec3(color.x, color.y, color.z), PreviewOffset);
}

void RailLinePreviewVisual::show(Node* sceneRoot, const RailNetwork& network, const RailLine* line,
	const vec3& color, const vec3& offset)
{
	if (!sceneRoot || !line)
	{
		clear();
		return;
	}
	ensureVisual(sceneRoot);
	if (!m_lineVisual)
	{
		return;
	}

	m_lineVisual->clear();
	if (line->isUsable)
	{
		appendLinePath(network, *line, color, offset);
	}

	const bool hasLines = m_lineVisual->getLineCount() > 0;
	m_lineVisual->setIsVisible(hasLines);
	if (hasLines)
	{
		m_lineVisual->initBuffer();
	}
}

void RailLinePreviewVisual::showOverview(Node* sceneRoot, const RailNetwork& network,
	const RailLineManager& lineManager, RailLineId selectedLineId)
{
	if (!sceneRoot)
	{
		clear();
		return;
	}
	ensureVisual(sceneRoot);
	if (!m_lineVisual)
	{
		return;
	}

	m_lineVisual->clear();
	for (const RailLine& line : lineManager.lines())
	{
		if (!line.isUsable)
		{
			continue;
		}
		const bool isSelected = line.id == selectedLineId;
		const vec4 color = isSelected ? railLineSelectedColor(line.id) : railLineColor(line.id);
		appendLinePath(network, line, vec3(color.x, color.y, color.z),
			isSelected ? SelectedOverviewOffset : OverviewOffset);
	}

	const bool hasLines = m_lineVisual->getLineCount() > 0;
	m_lineVisual->setIsVisible(hasLines);
	if (hasLines)
	{
		m_lineVisual->initBuffer();
	}
}

void RailLinePreviewVisual::clear()
{
	m_lineVisual = nullptr;
	if (m_visualRoot)
	{
		if (m_visualRoot->getParent())
		{
			m_visualRoot->removeFromParent();
		}
		delete m_visualRoot;
		m_visualRoot = nullptr;
	}
}

void RailLinePreviewVisual::appendLinePath(const RailNetwork& network, const RailLine& line,
	const vec3& color, const vec3& offset)
{
	for (const RailLinePathStep& step : line.pathSteps)
	{
		const RailSegment* segment = network.segment(step.segmentId);
		if (!segment || step.length <= 0.0001f)
		{
			continue;
		}

		const int lineSteps = std::max(1, static_cast<int>(std::ceil(step.length / PreviewSampleSpacing)));
		float prevDistance = step.segmentStartDistance;
		vec3 prev = segment->positionByDistance(prevDistance);
		for (int i = 1; i <= lineSteps; ++i)
		{
			const float local = step.length * (static_cast<float>(i) / static_cast<float>(lineSteps));
			const float ratio = step.length <= 0.0001f ? 0.0f : local / step.length;
			const float distance = step.segmentStartDistance
				+ (step.segmentEndDistance - step.segmentStartDistance) * ratio;
			const vec3 curr = segment->positionByDistance(distance);
			m_lineVisual->append(prev + offset, curr + offset, color);
			prev = curr;
		}
	}
}

void RailLinePreviewVisual::ensureVisual(Node* sceneRoot)
{
	if (!m_visualRoot)
	{
		m_visualRoot = new Node();
		m_visualRoot->setName("RailLinePreviewRoot");
	}
	if (!m_visualRoot->getParent())
	{
		sceneRoot->addChild(m_visualRoot, false);
	}
	if (!m_lineVisual)
	{
		m_lineVisual = new LinePrimitive();
		m_lineVisual->setIsAccpectOcTtree(false);
		m_lineVisual->setIsHitable(false);
		m_visualRoot->addChild(m_lineVisual, false);
	}
}

} // namespace tzw
