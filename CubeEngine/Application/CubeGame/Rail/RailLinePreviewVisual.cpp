#include "RailLinePreviewVisual.h"

#include "EngineSrc/3D/Primitive/LinePrimitive.h"
#include "EngineSrc/Base/Node.h"

#include <algorithm>
#include <cmath>

namespace tzw {

namespace
{
constexpr float PreviewSampleSpacing = 0.75f;
const vec3 PreviewOffset(0.0f, 0.16f, 0.0f);
const vec3 ValidLineColor(0.1f, 0.85f, 0.25f);
const vec3 InvalidLineColor(1.0f, 0.15f, 0.15f);
}

RailLinePreviewVisual::~RailLinePreviewVisual()
{
	clear();
}

void RailLinePreviewVisual::show(Node* sceneRoot, const RailNetwork& network, const RailLine* line)
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
		for (const RailLinePathStep& step : line->pathSteps)
		{
			const RailSegment* segment = network.segment(step.segmentId);
			if (!segment || step.length <= 0.0001f)
			{
				continue;
			}

			const int lineSteps = std::max(1, static_cast<int>(std::ceil(step.length / PreviewSampleSpacing)));
			float prevDistance = step.reversed ? step.length : 0.0f;
			vec3 prev = segment->positionByDistance(prevDistance);
			for (int i = 1; i <= lineSteps; ++i)
			{
				const float local = step.length * (static_cast<float>(i) / static_cast<float>(lineSteps));
				const float distance = step.reversed ? step.length - local : local;
				const vec3 curr = segment->positionByDistance(distance);
				m_lineVisual->append(prev + PreviewOffset, curr + PreviewOffset, ValidLineColor);
				prev = curr;
			}
		}
	}
	else
	{
		for (size_t i = 1; i < line->controlNodes.size(); ++i)
		{
			const RailNode* start = network.node(line->controlNodes[i - 1]);
			const RailNode* end = network.node(line->controlNodes[i]);
			if (!start || !end)
			{
				continue;
			}
			m_lineVisual->append(start->position + PreviewOffset, end->position + PreviewOffset, InvalidLineColor);
		}
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
