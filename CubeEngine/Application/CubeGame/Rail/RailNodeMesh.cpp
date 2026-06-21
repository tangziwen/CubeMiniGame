#include "RailNodeMesh.h"

namespace tzw {

void RailNodeMesh::build(const RailNetwork& network, const RailConfig& /*config*/, std::vector<RailNodeVisual>& outData) const
{
	outData.clear();

	for (const auto& pair : network.nodes())
	{
		const RailNode* railNode = pair.second.get();
		if (!railNode)
		{
			continue;
		}

		RailNodeVisual visual;
		visual.nodeId = railNode->id;
		visual.position = railNode->position + vec3(0.0f, 0.1f, 0.0f);
		outData.push_back(visual);
	}
}

} // namespace tzw
