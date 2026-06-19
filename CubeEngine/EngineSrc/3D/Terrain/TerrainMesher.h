#pragma once

#include "TerrainMeshRequest.h"
#include "TerrainOctreeNode.h"
#include "TerrainSampler.h"
#include "../../Mesh/Mesh.h"

#include <memory>

namespace tzw {

struct TerrainMeshBuildResult
{
	std::unique_ptr<Mesh> mesh;
	TerrainNodeKey key;
	TerrainRegion region;
	TerrainMeshState state = TerrainMeshState::Empty;
	bool isEmpty = true;
	AABB worldBounds;
};

class TerrainMesher
{
public:
	TerrainMeshBuildResult build(const TerrainMeshRequest& request,
		const TerrainSampleBuffer& sampleBuffer) const;
	void setDebugLodVertexColorEnabled(bool enabled);
	bool isDebugLodVertexColorEnabled() const;
	void setDebugLodCheckerboardEnabled(bool enabled);
	bool isDebugLodCheckerboardEnabled() const;

private:
	bool m_debugLodVertexColorEnabled = true;
	bool m_debugLodCheckerboardEnabled = true;
};

} // namespace tzw
