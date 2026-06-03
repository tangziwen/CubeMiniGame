#include "TerrainOctreeTypes.h"

#include <algorithm>
#include <sstream>

namespace tzw {

TerrainInt3::TerrainInt3(int theX, int theY, int theZ)
	: x(theX), y(theY), z(theZ)
{
}

bool TerrainInt3::operator==(const TerrainInt3& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

bool TerrainInt3::operator!=(const TerrainInt3& other) const
{
	return !(*this == other);
}

TerrainInt3 TerrainInt3::operator+(const TerrainInt3& other) const
{
	return TerrainInt3(x + other.x, y + other.y, z + other.z);
}

TerrainInt3 TerrainInt3::operator-(const TerrainInt3& other) const
{
	return TerrainInt3(x - other.x, y - other.y, z - other.z);
}

TerrainInt3 TerrainInt3::operator*(int value) const
{
	return TerrainInt3(x * value, y * value, z * value);
}

TerrainNodeKey::TerrainNodeKey(int theLevel, int theX, int theY, int theZ)
	: level(theLevel), x(theX), y(theY), z(theZ)
{
}

bool TerrainNodeKey::operator==(const TerrainNodeKey& other) const
{
	return level == other.level && x == other.x && y == other.y && z == other.z;
}

bool TerrainNodeKey::operator!=(const TerrainNodeKey& other) const
{
	return !(*this == other);
}

bool TerrainNodeKey::operator<(const TerrainNodeKey& other) const
{
	if (level != other.level)
	{
		return level < other.level;
	}
	if (x != other.x)
	{
		return x < other.x;
	}
	if (y != other.y)
	{
		return y < other.y;
	}
	return z < other.z;
}

bool TerrainNodeKey::isRoot() const
{
	return level == 0 && x == 0 && y == 0 && z == 0;
}

TerrainNodeKey TerrainNodeKey::parent() const
{
	if (level <= 0)
	{
		return TerrainNodeKey();
	}
	return TerrainNodeKey(level - 1, x / 2, y / 2, z / 2);
}

TerrainNodeKey TerrainNodeKey::child(int childIndex) const
{
	const int childX = childIndex & 1;
	const int childY = (childIndex >> 1) & 1;
	const int childZ = (childIndex >> 2) & 1;
	return TerrainNodeKey(level + 1, x * 2 + childX, y * 2 + childY, z * 2 + childZ);
}

bool TerrainNodeKey::isAncestorOf(const TerrainNodeKey& other) const
{
	if (level >= other.level)
	{
		return false;
	}

	const int shift = other.level - level;
	return x == (other.x >> shift)
		&& y == (other.y >> shift)
		&& z == (other.z >> shift);
}

bool TerrainNodeKey::isDescendantOf(const TerrainNodeKey& other) const
{
	return other.isAncestorOf(*this);
}

std::string TerrainNodeKey::toString() const
{
	std::ostringstream stream;
	stream << "L" << level << "(" << x << "," << y << "," << z << ")";
	return stream.str();
}

TerrainRegion::TerrainRegion(const TerrainInt3& theVoxelMin, int theCellSize)
	: voxelMin(theVoxelMin), cellSize(theCellSize)
{
}

TerrainInt3 TerrainRegion::cellMaxExclusive() const
{
	return voxelMin + TerrainInt3(cellSize, cellSize, cellSize);
}

TerrainInt3 TerrainRegion::sampleMin() const
{
	return voxelMin;
}

TerrainInt3 TerrainRegion::sampleMaxInclusive() const
{
	return cellMaxExclusive();
}

bool TerrainRegion::containsCell(const TerrainInt3& voxel) const
{
	const TerrainInt3 maxExclusive = cellMaxExclusive();
	return voxel.x >= voxelMin.x && voxel.y >= voxelMin.y && voxel.z >= voxelMin.z
		&& voxel.x < maxExclusive.x && voxel.y < maxExclusive.y && voxel.z < maxExclusive.z;
}

bool TerrainRegion::containsSample(const TerrainInt3& voxel) const
{
	const TerrainInt3 maxInclusive = sampleMaxInclusive();
	return voxel.x >= voxelMin.x && voxel.y >= voxelMin.y && voxel.z >= voxelMin.z
		&& voxel.x <= maxInclusive.x && voxel.y <= maxInclusive.y && voxel.z <= maxInclusive.z;
}

int TerrainRegion::sampleStride(int meshCellCount) const
{
	if (meshCellCount <= 0 || cellSize <= 0 || cellSize % meshCellCount != 0)
	{
		return 0;
	}
	return cellSize / meshCellCount;
}

int TerrainRegion::sampleLodPower(int meshCellCount) const
{
	const int stride = sampleStride(meshCellCount);
	if (!terrainIsPowerOfTwo(stride))
	{
		return -1;
	}
	return terrainLog2(stride);
}

vec3 TerrainRegion::worldMin(const vec3& mapOffset, float blockSize) const
{
	return mapOffset + vec3(static_cast<float>(voxelMin.x), static_cast<float>(voxelMin.y), static_cast<float>(voxelMin.z)) * blockSize;
}

vec3 TerrainRegion::worldMax(const vec3& mapOffset, float blockSize) const
{
	const TerrainInt3 maxExclusive = cellMaxExclusive();
	return mapOffset + vec3(static_cast<float>(maxExclusive.x), static_cast<float>(maxExclusive.y), static_cast<float>(maxExclusive.z)) * blockSize;
}

vec3 TerrainRegion::worldCenter(const vec3& mapOffset, float blockSize) const
{
	return (worldMin(mapOffset, blockSize) + worldMax(mapOffset, blockSize)) / 2.0f;
}

AABB TerrainRegion::worldAABB(const vec3& mapOffset, float blockSize) const
{
	AABB result;
	result.reset();
	result.update(worldMin(mapOffset, blockSize));
	result.update(worldMax(mapOffset, blockSize));
	return result;
}

TerrainOctreeConfig TerrainOctreeConfig::fromVoxelDomain(int widthVoxels, int heightVoxels, int depthVoxels,
	int theMeshCellCount, float theBlockSize, const vec3& theMapOffset)
{
	TerrainOctreeConfig result;
	const int meshCellCount = terrainNextPowerOfTwo(std::max(1, theMeshCellCount));
	const int maxDimension = std::max(widthVoxels, std::max(heightVoxels, depthVoxels));
	result.rootCellSize = terrainNextPowerOfTwo(std::max(maxDimension, meshCellCount));
	result.meshCellCount = meshCellCount;
	result.leafCellSize = meshCellCount;
	result.blockSize = theBlockSize;
	result.mapOffset = theMapOffset;
	result.maxDepth = terrainLog2(result.rootCellSize / result.leafCellSize);
	return result;
}

bool TerrainOctreeConfig::isValid() const
{
	if (rootCellSize <= 0 || leafCellSize <= 0 || meshCellCount <= 0 || blockSize <= 0.0f)
	{
		return false;
	}
	if (!terrainIsPowerOfTwo(rootCellSize) || !terrainIsPowerOfTwo(leafCellSize) || !terrainIsPowerOfTwo(meshCellCount))
	{
		return false;
	}
	if (leafCellSize != meshCellCount || rootCellSize < leafCellSize || rootCellSize % leafCellSize != 0)
	{
		return false;
	}
	return maxDepth == terrainLog2(rootCellSize / leafCellSize);
}

int TerrainOctreeConfig::cellSizeForLevel(int level) const
{
	if (level < 0 || level > maxDepth)
	{
		return 0;
	}
	return rootCellSize >> level;
}

TerrainRegion TerrainOctreeConfig::rootRegion() const
{
	return TerrainRegion(TerrainInt3(0, 0, 0), rootCellSize);
}

TerrainRegion TerrainOctreeConfig::regionForKey(const TerrainNodeKey& key) const
{
	const int nodeCellSize = cellSizeForLevel(key.level);
	return TerrainRegion(TerrainInt3(key.x * nodeCellSize, key.y * nodeCellSize, key.z * nodeCellSize), nodeCellSize);
}

bool terrainIsPowerOfTwo(int value)
{
	return value > 0 && (value & (value - 1)) == 0;
}

int terrainNextPowerOfTwo(int value)
{
	if (value <= 1)
	{
		return 1;
	}

	int result = 1;
	while (result < value)
	{
		result <<= 1;
	}
	return result;
}

int terrainLog2(int value)
{
	if (value <= 0)
	{
		return -1;
	}

	int result = 0;
	while (value > 1)
	{
		value >>= 1;
		++result;
	}
	return result;
}

} // namespace tzw

namespace std {

size_t hash<tzw::TerrainInt3>::operator()(const tzw::TerrainInt3& value) const noexcept
{
	size_t seed = 1469598103934665603ull;
	seed ^= static_cast<size_t>(value.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= static_cast<size_t>(value.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= static_cast<size_t>(value.z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	return seed;
}

size_t hash<tzw::TerrainNodeKey>::operator()(const tzw::TerrainNodeKey& value) const noexcept
{
	size_t seed = 1469598103934665603ull;
	seed ^= static_cast<size_t>(value.level) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= static_cast<size_t>(value.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= static_cast<size_t>(value.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= static_cast<size_t>(value.z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	return seed;
}

} // namespace std
