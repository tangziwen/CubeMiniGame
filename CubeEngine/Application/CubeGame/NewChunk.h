#pragma once
#include "EngineSrc/CubeEngine.h"
#include <vector>
#include "3D/Vegetation/Grass.h"
#include "3D/Vegetation/Tree.h"
#include "GameMap.h"

namespace tzw
{
	class PhysicsRigidBody;
	class ChunkInfo;
	
	class NewChunk : public Drawable3D
	{
	public:
		NewChunk(int x, int y, int z);
		void submitDrawCmd(RenderCommand::RenderType passType) override;
		void gen(int lodLevel);
	private:
		int x,y,z;
		 Mesh * m_mesh;
		vec3 m_basePoint;
	};
}
