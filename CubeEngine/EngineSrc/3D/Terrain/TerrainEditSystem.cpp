#include "TerrainEditSystem.h"

#include "CubeGame/GameMap.h"
#include "TerrainOctree.h"
#include "TerrainMeshCache.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace tzw {

TerrainEditSystem::TerrainEditSystem()
	: m_revision(0)
{
}

void TerrainEditSystem::bind(GameMap* map, TerrainOctree* octree, TerrainMeshCache* cache)
{
	m_map = map;
	m_octree = octree;
	m_cache = cache;
}

int TerrainEditSystem::currentRevision() const
{
	return m_revision;
}

TerrainEditResult TerrainEditSystem::apply(const TerrainEditOperation& operation)
{
	if (!m_map)
	{
		return TerrainEditResult();
	}
	return applyInternal(operation);
}

TerrainEditResult TerrainEditSystem::deformSphere(vec3 centerWorld, float radiusWorld, float value)
{
	TerrainEditOperation op;
	op.mode = TerrainEditMode::AddDensity;
	op.shape = TerrainEditShape::Sphere;
	op.centerWorld = centerWorld;
	op.radiusWorld = radiusWorld;
	op.densityValue = value;
	op.additive = true;
	return apply(op);
}

TerrainEditResult TerrainEditSystem::deformBox(vec3 centerWorld, vec3 halfExtentsWorld, float value)
{
	TerrainEditOperation op;
	op.mode = TerrainEditMode::AddDensity;
	op.shape = TerrainEditShape::Box;
	op.centerWorld = centerWorld;
	op.halfExtentsWorld = halfExtentsWorld;
	op.densityValue = value;
	op.additive = true;
	return apply(op);
}

TerrainEditResult TerrainEditSystem::paintSphere(vec3 centerWorld, float radiusWorld, int materialIndex)
{
	TerrainEditOperation op;
	op.mode = TerrainEditMode::PaintMaterial;
	op.shape = TerrainEditShape::Sphere;
	op.centerWorld = centerWorld;
	op.radiusWorld = radiusWorld;
	op.materialIndex = materialIndex;
	op.additive = true;
	return apply(op);
}

TerrainEditResult TerrainEditSystem::applyInternal(const TerrainEditOperation& operation)
{
	TerrainEditResult result;
	if (!m_map)
	{
		return result;
	}
	if (operation.shape == TerrainEditShape::Sphere && operation.radiusWorld <= 0.0f)
	{
		return result;
	}
	if (operation.shape == TerrainEditShape::Box
		&& (operation.halfExtentsWorld.x < 0.0f || operation.halfExtentsWorld.y < 0.0f || operation.halfExtentsWorld.z < 0.0f))
	{
		return result;
	}

	const vec3 mapOffset = m_map->getMapOffset();
	const float blockSize = BLOCK_SIZE;

	TerrainEditBounds bounds;
	if (operation.shape == TerrainEditShape::Box)
	{
		bounds.voxelMin.x = static_cast<int>(std::floor((operation.centerWorld.x - operation.halfExtentsWorld.x - mapOffset.x) / blockSize));
		bounds.voxelMin.y = static_cast<int>(std::floor((operation.centerWorld.y - operation.halfExtentsWorld.y - mapOffset.y) / blockSize));
		bounds.voxelMin.z = static_cast<int>(std::floor((operation.centerWorld.z - operation.halfExtentsWorld.z - mapOffset.z) / blockSize));
		bounds.voxelMaxExclusive.x = static_cast<int>(std::floor((operation.centerWorld.x + operation.halfExtentsWorld.x - mapOffset.x) / blockSize)) + 1;
		bounds.voxelMaxExclusive.y = static_cast<int>(std::floor((operation.centerWorld.y + operation.halfExtentsWorld.y - mapOffset.y) / blockSize)) + 1;
		bounds.voxelMaxExclusive.z = static_cast<int>(std::floor((operation.centerWorld.z + operation.halfExtentsWorld.z - mapOffset.z) / blockSize)) + 1;
	}
	else // Sphere
	{
		const float vx = (operation.centerWorld.x - mapOffset.x) / blockSize;
		const float vy = (operation.centerWorld.y - mapOffset.y) / blockSize;
		const float vz = (operation.centerWorld.z - mapOffset.z) / blockSize;
		const int centerX = static_cast<int>(std::floor(vx));
		const int centerY = static_cast<int>(std::floor(vy));
		const int centerZ = static_cast<int>(std::floor(vz));
		const int searchSize = static_cast<int>(std::ceil(operation.radiusWorld / blockSize)) + 1;
		bounds.voxelMin = TerrainInt3(centerX - searchSize, centerY - searchSize, centerZ - searchSize);
		bounds.voxelMaxExclusive = TerrainInt3(centerX + searchSize + 1, centerY + searchSize + 1, centerZ + searchSize + 1);
	}

	TerrainEditBounds affectedBounds;
	affectedBounds.voxelMin = TerrainInt3(
		std::numeric_limits<int>::max(),
		std::numeric_limits<int>::max(),
		std::numeric_limits<int>::max());
	affectedBounds.voxelMaxExclusive = TerrainInt3(
		std::numeric_limits<int>::lowest(),
		std::numeric_limits<int>::lowest(),
		std::numeric_limits<int>::lowest());
	auto recordAffectedVoxel = [&affectedBounds](int x, int y, int z)
	{
		affectedBounds.voxelMin.x = std::min(affectedBounds.voxelMin.x, x);
		affectedBounds.voxelMin.y = std::min(affectedBounds.voxelMin.y, y);
		affectedBounds.voxelMin.z = std::min(affectedBounds.voxelMin.z, z);
		affectedBounds.voxelMaxExclusive.x = std::max(affectedBounds.voxelMaxExclusive.x, x + 1);
		affectedBounds.voxelMaxExclusive.y = std::max(affectedBounds.voxelMaxExclusive.y, y + 1);
		affectedBounds.voxelMaxExclusive.z = std::max(affectedBounds.voxelMaxExclusive.z, z + 1);
	};

	for (int x = bounds.voxelMin.x; x < bounds.voxelMaxExclusive.x; ++x)
	{
		for (int y = bounds.voxelMin.y; y < bounds.voxelMaxExclusive.y; ++y)
		{
			for (int z = bounds.voxelMin.z; z < bounds.voxelMaxExclusive.z; ++z)
			{
				if (!m_map->isVoxelInDomain(x, y, z))
				{
					continue;
				}

				if (operation.shape == TerrainEditShape::Sphere)
				{
					const vec3 voxelWorldPos = mapOffset + vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) * blockSize;
					const float dist = voxelWorldPos.distance(operation.centerWorld);
					if (dist > operation.radiusWorld)
					{
						continue;
					}

					const float falloff = (operation.radiusWorld - dist) / operation.radiusWorld;

					if (operation.mode == TerrainEditMode::AddDensity)
					{
						const float delta = operation.densityValue * falloff * 128.0f;
						voxelInfo* v = m_map->ensureVoxelForWrite(x, y, z);
						if (v)
						{
							int newW = static_cast<int>(static_cast<short>(v->w) + static_cast<short>(delta));
							newW = std::clamp(newW, 0, 255);
							m_map->writeVoxelDensity(x, y, z, static_cast<unsigned char>(newW));
							++result.editedVoxelCount;
							result.changed = true;
							recordAffectedVoxel(x, y, z);
						}
					}
					else if (operation.mode == TerrainEditMode::SetDensity)
					{
						int newW = static_cast<int>(std::round(operation.densityValue * falloff));
						newW = std::clamp(newW, 0, 255);
						m_map->writeVoxelDensity(x, y, z, static_cast<unsigned char>(newW));
						++result.editedVoxelCount;
						result.changed = true;
						recordAffectedVoxel(x, y, z);
					}
					else if (operation.mode == TerrainEditMode::PaintMaterial)
					{
						m_map->writeVoxelMaterial(x, y, z, operation.materialIndex);
						++result.editedVoxelCount;
						result.changed = true;
						recordAffectedVoxel(x, y, z);
					}
				}
				else // Box
				{
					if (operation.mode == TerrainEditMode::AddDensity)
					{
						const float delta = operation.densityValue * 128.0f;
						voxelInfo* v = m_map->ensureVoxelForWrite(x, y, z);
						if (v)
						{
							int newW = static_cast<int>(static_cast<short>(v->w) + static_cast<short>(delta));
							newW = std::clamp(newW, 0, 255);
							m_map->writeVoxelDensity(x, y, z, static_cast<unsigned char>(newW));
							++result.editedVoxelCount;
							result.changed = true;
							recordAffectedVoxel(x, y, z);
						}
					}
					else if (operation.mode == TerrainEditMode::SetDensity)
					{
						int newW = static_cast<int>(std::round(operation.densityValue));
						newW = std::clamp(newW, 0, 255);
						m_map->writeVoxelDensity(x, y, z, static_cast<unsigned char>(newW));
						++result.editedVoxelCount;
						result.changed = true;
						recordAffectedVoxel(x, y, z);
					}
					else if (operation.mode == TerrainEditMode::PaintMaterial)
					{
						m_map->writeVoxelMaterial(x, y, z, operation.materialIndex);
						++result.editedVoxelCount;
						result.changed = true;
						recordAffectedVoxel(x, y, z);
					}
				}
			}
		}
	}

	if (result.changed)
	{
		++m_revision;
		result.revision = m_revision;
		result.affectedBounds = affectedBounds;

		if (m_octree)
		{
			result.dirtiedNodeCount = m_octree->markDirtyInBounds(affectedBounds);
		}
		if (m_cache && m_octree)
		{
			m_cache->invalidateInBounds(affectedBounds, m_octree->config());
		}
	}

	return result;
}

} // namespace tzw
