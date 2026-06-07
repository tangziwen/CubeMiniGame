#pragma once

#include "../../Engine/EngineDef.h"
#include "../../Mesh/Mesh.h"

namespace tzw {
	struct voxelInfo;

	struct SurfaceNetsGenerateConfig
	{
		int voxelSize = 0;
		int cellCount = 0;
		int minPadding = 0;
		float cellWorldSize = 1.0f;
		unsigned char isoLevel = 128;
	};

	class SurfaceNets : public Singleton<SurfaceNets>
	{
	public:
		SurfaceNets();
		void generate(vec3 basePoint, Mesh* mesh, const voxelInfo* srcData,
			const SurfaceNetsGenerateConfig& config);
	};
}
