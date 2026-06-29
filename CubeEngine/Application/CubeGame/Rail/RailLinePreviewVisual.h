#pragma once

#include "RailLine.h"

namespace tzw {

class LinePrimitive;
class Node;

class RailLinePreviewVisual
{
public:
	~RailLinePreviewVisual();

	void show(Node* sceneRoot, const RailNetwork& network, const RailLine* line);
	void show(Node* sceneRoot, const RailNetwork& network, const RailLine* line, const vec3& color,
		const vec3& offset);
	void showOverview(Node* sceneRoot, const RailNetwork& network, const RailLineManager& lineManager,
		RailLineId selectedLineId);
	void clear();

private:
	void ensureVisual(Node* sceneRoot);
	void appendLinePath(const RailNetwork& network, const RailLine& line, const vec3& color, const vec3& offset);

	Node* m_visualRoot = nullptr;
	LinePrimitive* m_lineVisual = nullptr;
};

} // namespace tzw
