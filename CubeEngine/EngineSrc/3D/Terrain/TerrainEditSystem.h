#pragma once

#include "TerrainOctreeTypes.h"

namespace tzw {

class GameMap;
class TerrainOctree;
class TerrainMeshCache;

enum class TerrainEditMode
{
	AddDensity,
	SetDensity,
	PaintMaterial,
};

enum class TerrainEditShape
{
	Sphere,
	Box,
};

struct TerrainEditOperation
{
	TerrainEditMode mode = TerrainEditMode::AddDensity;
	TerrainEditShape shape = TerrainEditShape::Sphere;
	vec3 centerWorld;
	vec3 halfExtentsWorld;
	float radiusWorld = 0.0f;
	float densityValue = 0.0f;
	int materialIndex = 0;
	bool additive = true;
};

struct TerrainEditResult
{
	bool changed = false;
	TerrainEditBounds affectedBounds;
	int revision = 0;
	int editedVoxelCount = 0;
	int dirtiedNodeCount = 0;
};

class TerrainEditSystem
{
public:
	TerrainEditSystem();

	void bind(GameMap* map, TerrainOctree* octree, TerrainMeshCache* cache);

	TerrainEditResult apply(const TerrainEditOperation& operation);
	TerrainEditResult deformSphere(vec3 centerWorld, float radiusWorld, float value);
	TerrainEditResult deformBox(vec3 centerWorld, vec3 halfExtentsWorld, float value);
	TerrainEditResult paintSphere(vec3 centerWorld, float radiusWorld, int materialIndex);

	bool raycast(const vec3& origin, const vec3& dir, float maxDist, vec3& outHit) const;

	int currentRevision() const;

private:
	TerrainEditResult applyInternal(const TerrainEditOperation& operation);

	GameMap* m_map = nullptr;
	TerrainOctree* m_octree = nullptr;
	TerrainMeshCache* m_cache = nullptr;
	int m_revision = 0;
};

} // namespace tzw
