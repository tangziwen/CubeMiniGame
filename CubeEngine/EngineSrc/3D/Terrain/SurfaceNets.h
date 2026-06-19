#pragma once

#include "../../Engine/EngineDef.h"
#include "../../Mesh/Mesh.h"
#include "TerrainMeshRequest.h"

namespace tzw {
	struct voxelInfo;

	struct SurfaceNetsGenerateConfig
	{
		int voxelSize = 0;
		int cellCount = 0;
		int minPadding = 0;
		float cellWorldSize = 1.0f;
		unsigned char isoLevel = 128;

		TerrainMeshSeamSet seams;
		TerrainInt3 sampleOrigin;
		int sampleStride = 0;
		TerrainInt3 domainSize;
		vec3 mapOffset;
		float blockSize = 1.0f;

		bool debugLodVertexColorEnabled = true;
		int debugLodPower = 0;
	};

	class SurfaceNets : public Singleton<SurfaceNets>
	{
	public:
		SurfaceNets();
		void generate(vec3 basePoint, Mesh* mesh, const voxelInfo* srcData,
			const SurfaceNetsGenerateConfig& config);
	};
}
