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
	void clear();

private:
	void ensureVisual(Node* sceneRoot);

	Node* m_visualRoot = nullptr;
	LinePrimitive* m_lineVisual = nullptr;
};

} // namespace tzw
