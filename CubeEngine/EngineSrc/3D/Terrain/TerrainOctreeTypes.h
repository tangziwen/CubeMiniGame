#pragma once

#include "../../Math/AABB.h"
#include "../../Math/vec3.h"

#include <cstddef>
#include <functional>
#include <string>

namespace tzw {

struct TerrainInt3
{
	int x = 0;
	int y = 0;
	int z = 0;

	TerrainInt3() = default;
	TerrainInt3(int theX, int theY, int theZ);

	bool operator==(const TerrainInt3& other) const;
	bool operator!=(const TerrainInt3& other) const;
	TerrainInt3 operator+(const TerrainInt3& other) const;
	TerrainInt3 operator-(const TerrainInt3& other) const;
	TerrainInt3 operator*(int value) const;
};

struct TerrainNodeKey
{
	int level = 0;
	int x = 0;
	int y = 0;
	int z = 0;

	TerrainNodeKey() = default;
	TerrainNodeKey(int theLevel, int theX, int theY, int theZ);

	bool operator==(const TerrainNodeKey& other) const;
	bool operator!=(const TerrainNodeKey& other) const;
	bool operator<(const TerrainNodeKey& other) const;

	bool isRoot() const;
	TerrainNodeKey parent() const;
	TerrainNodeKey child(int childIndex) const;
	bool isAncestorOf(const TerrainNodeKey& other) const;
	bool isDescendantOf(const TerrainNodeKey& other) const;
	std::string toString() const;
};

struct TerrainRegion
{
	TerrainInt3 voxelMin;
	int cellSize = 0;

	TerrainRegion() = default;
	TerrainRegion(const TerrainInt3& theVoxelMin, int theCellSize);

	TerrainInt3 cellMaxExclusive() const;
	TerrainInt3 sampleMin() const;
	TerrainInt3 sampleMaxInclusive() const;
	bool containsCell(const TerrainInt3& voxel) const;
	bool containsSample(const TerrainInt3& voxel) const;
	int sampleStride(int meshCellCount) const;
	int sampleLodPower(int meshCellCount) const;
	vec3 worldMin(const vec3& mapOffset, float blockSize) const;
	vec3 worldMax(const vec3& mapOffset, float blockSize) const;
	vec3 worldCenter(const vec3& mapOffset, float blockSize) const;
	AABB worldAABB(const vec3& mapOffset, float blockSize) const;
};

struct TerrainOctreeConfig
{
	int rootCellSize = 16;
	int leafCellSize = 16;
	int meshCellCount = 16;
	int maxDepth = 0;
	float blockSize = 1.0f;
	vec3 mapOffset;

	static TerrainOctreeConfig fromVoxelDomain(int widthVoxels, int heightVoxels, int depthVoxels,
		int theMeshCellCount, float theBlockSize, const vec3& theMapOffset);

	bool isValid() const;
	int cellSizeForLevel(int level) const;
	TerrainRegion rootRegion() const;
	TerrainRegion regionForKey(const TerrainNodeKey& key) const;
};

struct TerrainEditBounds
{
	TerrainInt3 voxelMin;
	TerrainInt3 voxelMaxExclusive;
};

bool terrainIsPowerOfTwo(int value);
int terrainNextPowerOfTwo(int value);
int terrainLog2(int value);

} // namespace tzw

namespace std {

template<>
struct hash<tzw::TerrainInt3>
{
	size_t operator()(const tzw::TerrainInt3& value) const noexcept;
};

template<>
struct hash<tzw::TerrainNodeKey>
{
	size_t operator()(const tzw::TerrainNodeKey& value) const noexcept;
};

} // namespace std
