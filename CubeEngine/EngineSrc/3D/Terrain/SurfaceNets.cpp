#include "SurfaceNets.h"

#include "GameMap.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <vector>

namespace tzw {

namespace {
	float convertToRange(unsigned char input)
	{
		return (static_cast<float>(input) / 255.0f) * 2.0f - 1.0f;
	}

	int getIndex(int x, int y, int z, int voxelSize)
	{
		return z + y * voxelSize + x * voxelSize * voxelSize;
	}

	unsigned char getValue(const voxelInfo* srcData, int voxelSize, int x, int y, int z)
	{
		return srcData[getIndex(x, y, z, voxelSize)].w;
	}

	const voxelInfo* getVoxel(const voxelInfo* srcData, int voxelSize, int x, int y, int z)
	{
		return srcData + getIndex(x, y, z, voxelSize);
	}

	int axisValue(const TerrainInt3& value, int axis)
	{
		switch (axis)
		{
		case 0:
			return value.x;
		case 1:
			return value.y;
		case 2:
			return value.z;
		default:
			return 0;
		}
	}

	bool samePosition(const vec3& a, const vec3& b)
	{
		return (a - b).squaredLength() <= 1e-8f;
	}

	const std::array<std::array<std::array<int, 3>, 2>, 12>& edgePairs()
	{
		static const std::array<std::array<std::array<int, 3>, 2>, 12> pairs = {{
			{{{{0, 0, 0}}, {{1, 0, 0}}}},
			{{{{1, 0, 0}}, {{1, 1, 0}}}},
			{{{{1, 1, 0}}, {{0, 1, 0}}}},
			{{{{0, 1, 0}}, {{0, 0, 0}}}},
			{{{{0, 0, 1}}, {{1, 0, 1}}}},
			{{{{1, 0, 1}}, {{1, 1, 1}}}},
			{{{{1, 1, 1}}, {{0, 1, 1}}}},
			{{{{0, 1, 1}}, {{0, 0, 1}}}},
			{{{{0, 0, 0}}, {{0, 0, 1}}}},
			{{{{1, 0, 0}}, {{1, 0, 1}}}},
			{{{{1, 1, 0}}, {{1, 1, 1}}}},
			{{{{0, 1, 0}}, {{0, 1, 1}}}}
		}};
		return pairs;
	}

	vec3 interpolate(vec3 p1, unsigned char v1Char, vec3 p2, unsigned char v2Char)
	{
		const float v1 = convertToRange(v1Char);
		const float v2 = convertToRange(v2Char);
		if (std::abs(v1 - v2) < 1e-5f)
		{
			return p1;
		}
		const float t = (0.0f - v1) / (v2 - v1);
		return vec3(p1.x + t * (p2.x - p1.x), p1.y + t * (p2.y - p1.y), p1.z + t * (p2.z - p1.z));
	}

	VertexData makeVertexData(vec3 position, const voxelInfo& voxel)
	{
		VertexData vertex(position);
		vertex.m_matIndex[0] = voxel.matInfo.matIndex1;
		vertex.m_matIndex[1] = voxel.matInfo.matIndex2;
		vertex.m_matIndex[2] = voxel.matInfo.matIndex3;
		const float factor1 = HIGH_FOUR_BIT(voxel.matInfo.matBlendFactor) / 15.f;
		const float factor2 = LOW_FOUR_BIT(voxel.matInfo.matBlendFactor) / 15.f;
		vertex.m_matBlendFactor = vec3(factor1, factor2, 1.f - factor1 - factor2);
		vertex.m_texCoord = position.xz();
		return vertex;
	}

	struct DualVertexResult
	{
		bool exists = false;
		vec3 localCenter;
		vec3 worldPosition;
		voxelInfo material;
	};

	DualVertexResult calculateDualVertex(vec3 basePoint, const voxelInfo* srcData, int voxelSize,
		int minCell, float cellWorldSize, unsigned char isoLevel, int x, int y, int z)
	{
		DualVertexResult result;

		const unsigned char corners[8] = {
			getValue(srcData, voxelSize, x, y, z),
			getValue(srcData, voxelSize, x + 1, y, z),
			getValue(srcData, voxelSize, x + 1, y + 1, z),
			getValue(srcData, voxelSize, x, y + 1, z),
			getValue(srcData, voxelSize, x, y, z + 1),
			getValue(srcData, voxelSize, x + 1, y, z + 1),
			getValue(srcData, voxelSize, x + 1, y + 1, z + 1),
			getValue(srcData, voxelSize, x, y + 1, z + 1)
		};

		int mask = 0;
		for (int i = 0; i < 8; ++i)
		{
			if (corners[i] < isoLevel) mask |= (1 << i);
		}
		if (mask == 0 || mask == 0xff)
		{
			return result;
		}

		vec3 sum(0, 0, 0);
		int count = 0;
		for (const auto& edge : edgePairs())
		{
			const int dx1 = edge[0][0], dy1 = edge[0][1], dz1 = edge[0][2];
			const int dx2 = edge[1][0], dy2 = edge[1][1], dz2 = edge[1][2];
			const unsigned char v1 = getValue(srcData, voxelSize, x + dx1, y + dy1, z + dz1);
			const unsigned char v2 = getValue(srcData, voxelSize, x + dx2, y + dy2, z + dz2);
			if ((v1 < isoLevel) != (v2 < isoLevel))
			{
				const vec3 p1(static_cast<float>(x + dx1), static_cast<float>(y + dy1), static_cast<float>(z + dz1));
				const vec3 p2(static_cast<float>(x + dx2), static_cast<float>(y + dy2), static_cast<float>(z + dz2));
				sum += interpolate(p1, v1, p2, v2);
				++count;
			}
		}
		if (count == 0)
		{
			return result;
		}

		const vec3 center = sum / static_cast<float>(count);
		const vec3 finalPosition = basePoint
			+ (center - vec3(static_cast<float>(minCell)))
			* cellWorldSize;
		const int matX = std::clamp(static_cast<int>(center.x), 0, voxelSize - 1);
		const int matY = std::clamp(static_cast<int>(center.y), 0, voxelSize - 1);
		const int matZ = std::clamp(static_cast<int>(center.z), 0, voxelSize - 1);

		result.exists = true;
		result.localCenter = center;
		result.worldPosition = finalPosition;
		result.material = *getVoxel(srcData, voxelSize, matX, matY, matZ);
		return result;
	}

	void addTriangleIfValid(Mesh* mesh, int i0, int i1, int i2)
	{
		if (i0 < 0 || i1 < 0 || i2 < 0)
		{
			return;
		}
		if (i0 == i1 || i1 == i2 || i0 == i2)
		{
			return;
		}
		const vec3& p0 = mesh->m_vertices[i0].m_pos;
		const vec3& p1 = mesh->m_vertices[i1].m_pos;
		const vec3& p2 = mesh->m_vertices[i2].m_pos;
		if (samePosition(p0, p1) || samePosition(p1, p2) || samePosition(p0, p2))
		{
			return;
		}
		mesh->addIndex(i0);
		mesh->addIndex(i1);
		mesh->addIndex(i2);
	}

	vec3 estimateDensityGradient(const SurfaceNetsGenerateConfig& config, const vec3& worldPosition)
	{
		if (config.blockSize <= 0.0f)
		{
			return vec3(0, 0, 0);
		}

		const vec3 local = (worldPosition - config.mapOffset) / config.blockSize;
		const int x = static_cast<int>(std::floor(local.x + 0.5f));
		const int y = static_cast<int>(std::floor(local.y + 0.5f));
		const int z = static_cast<int>(std::floor(local.z + 0.5f));
		GameMap* map = GameMap::shared();
		const float dx = static_cast<float>(map->sampleVoxel(x + 1, y, z).w)
			- static_cast<float>(map->sampleVoxel(x - 1, y, z).w);
		const float dy = static_cast<float>(map->sampleVoxel(x, y + 1, z).w)
			- static_cast<float>(map->sampleVoxel(x, y - 1, z).w);
		const float dz = static_cast<float>(map->sampleVoxel(x, y, z + 1).w)
			- static_cast<float>(map->sampleVoxel(x, y, z - 1).w);
		return vec3(dx, dy, dz);
	}

	void addTransitionTriangleIfValid(Mesh* mesh, int i0, int i1, int i2,
		const SurfaceNetsGenerateConfig& config)
	{
		if (i0 < 0 || i1 < 0 || i2 < 0)
		{
			return;
		}
		if (i0 == i1 || i1 == i2 || i0 == i2)
		{
			return;
		}
		const vec3& p0 = mesh->m_vertices[i0].m_pos;
		const vec3& p1 = mesh->m_vertices[i1].m_pos;
		const vec3& p2 = mesh->m_vertices[i2].m_pos;
		if (samePosition(p0, p1) || samePosition(p1, p2) || samePosition(p0, p2))
		{
			return;
		}

		const vec3 normal = vec3::CrossProduct(p1 - p0, p2 - p0);
		const vec3 center = (p0 + p1 + p2) / 3.0f;
		const vec3 gradient = estimateDensityGradient(config, center);
		if (normal.squaredLength() > 1e-8f && gradient.squaredLength() > 1e-8f
			&& vec3::DotProduct(normal, gradient) < 0.0f)
		{
			std::swap(i1, i2);
		}
		addTriangleIfValid(mesh, i0, i1, i2);
	}

	void addQuad(Mesh* mesh, int c1, int c2, int c3, int c4, bool flipWinding)
	{
		if (c1 == -1 || c2 == -1 || c3 == -1 || c4 == -1)
		{
			return;
		}
		if (flipWinding)
		{
			addTriangleIfValid(mesh, c1, c3, c2);
			addTriangleIfValid(mesh, c1, c4, c3);
		}
		else
		{
			addTriangleIfValid(mesh, c1, c2, c3);
			addTriangleIfValid(mesh, c1, c3, c4);
		}
	}

	struct FaceInfo
	{
		int axis = 0;
		int side = 0;
		int tangential[2] = { 1, 2 };
	};

	FaceInfo getFaceInfo(int faceIndex)
	{
		FaceInfo info;
		info.axis = faceIndex / 2;
		info.side = (faceIndex % 2 == 0) ? -1 : +1;
		info.tangential[0] = (info.axis + 1) % 3;
		info.tangential[1] = (info.axis + 2) % 3;
		return info;
	}

	voxelInfo sampleGlobalVoxelAtLocalCell(const SurfaceNetsGenerateConfig& config, const int localCell[3])
	{
		const TerrainInt3 globalCell(
			config.sampleOrigin.x + localCell[0] * config.sampleStride,
			config.sampleOrigin.y + localCell[1] * config.sampleStride,
			config.sampleOrigin.z + localCell[2] * config.sampleStride);
		return GameMap::shared()->sampleVoxel(
			globalCell.x,
			globalCell.y,
			globalCell.z);
	}

	DualVertexResult calculateCoarseDualVertexForFineCell(const SurfaceNetsGenerateConfig& config,
		const int localCell[3])
	{
		DualVertexResult result;

		const int s = config.sampleStride;
		const unsigned char isoLevel = config.isoLevel;
		if (s <= 0)
		{
			return result;
		}

		const int coarseCell[3] = {
			static_cast<int>(std::floor((config.sampleOrigin.x + localCell[0] * s) / (2.0f * s))),
			static_cast<int>(std::floor((config.sampleOrigin.y + localCell[1] * s) / (2.0f * s))),
			static_cast<int>(std::floor((config.sampleOrigin.z + localCell[2] * s) / (2.0f * s)))
		};

		const int coarseOrigin[3] = {
			coarseCell[0] * 2 * s,
			coarseCell[1] * 2 * s,
			coarseCell[2] * 2 * s
		};

		GameMap* map = GameMap::shared();
		auto sampleCoarseCorner = [&](int dx, int dy, int dz) -> unsigned char
		{
			return map->sampleVoxel(
				coarseOrigin[0] + dx * 2 * s,
				coarseOrigin[1] + dy * 2 * s,
				coarseOrigin[2] + dz * 2 * s).w;
		};

		int insideCount = 0;
		for (int dz = 0; dz <= 1; ++dz)
		{
			for (int dy = 0; dy <= 1; ++dy)
			{
				for (int dx = 0; dx <= 1; ++dx)
				{
					if (sampleCoarseCorner(dx, dy, dz) < isoLevel)
					{
						++insideCount;
					}
				}
			}
		}
		if (insideCount == 0 || insideCount == 8)
		{
			return result;
		}

		vec3 sum(0, 0, 0);
		int count = 0;
		for (const auto& edge : edgePairs())
		{
			const unsigned char v1 = sampleCoarseCorner(edge[0][0], edge[0][1], edge[0][2]);
			const unsigned char v2 = sampleCoarseCorner(edge[1][0], edge[1][1], edge[1][2]);
			if ((v1 < isoLevel) != (v2 < isoLevel))
			{
				const vec3 p1(
					static_cast<float>(coarseOrigin[0] + edge[0][0] * 2 * s),
					static_cast<float>(coarseOrigin[1] + edge[0][1] * 2 * s),
					static_cast<float>(coarseOrigin[2] + edge[0][2] * 2 * s));
				const vec3 p2(
					static_cast<float>(coarseOrigin[0] + edge[1][0] * 2 * s),
					static_cast<float>(coarseOrigin[1] + edge[1][1] * 2 * s),
					static_cast<float>(coarseOrigin[2] + edge[1][2] * 2 * s));
				sum += interpolate(p1, v1, p2, v2);
				++count;
			}
		}
		if (count == 0)
		{
			return result;
		}

		const vec3 centerGlobal = sum / static_cast<float>(count);
		result.exists = true;
		result.worldPosition = config.mapOffset + centerGlobal * config.blockSize;
		return result;
	}

	int addStitchedVertex(Mesh* mesh, const vec3& worldPosition, const voxelInfo& material)
	{
		mesh->addVertex(makeVertexData(worldPosition, material));
		return static_cast<int>(mesh->getVerticesSize()) - 1;
	}

	void emitTransitionStripForFace(Mesh* mesh, const SurfaceNetsGenerateConfig& config,
		std::vector<int>& cellVertexIndex, int faceIndex)
	{
		const FaceInfo face = getFaceInfo(faceIndex);
		const int axis = face.axis;
		const int side = face.side;
		const int uAxis = face.tangential[0];
		const int vAxis = face.tangential[1];

		const int minCell = config.minPadding;
		const int maxCell = minCell + config.cellCount;
		// Match the same cell layer that Pass 2 would use for a boundary face.
		// Negative faces are owned by the halo layer; positive faces use the last
		// interior layer because the opposite chunk owns the matching negative face.
		const int faceLocal = (side < 0) ? (minCell - 1) : (maxCell - 1);
		const int materialLocal = std::clamp(faceLocal, minCell, maxCell - 1);
		const int voxelSize = config.voxelSize;
		const int s = config.sampleStride;

		// Group face fine cells by coarse cell index in the tangential plane.
		std::map<std::pair<int, int>, std::vector<std::pair<int, int>>> coarseToFine;
		for (int u = minCell; u < maxCell; ++u)
		{
			for (int v = minCell; v < maxCell; ++v)
			{
				const int cu = static_cast<int>(std::floor(
					(axisValue(config.sampleOrigin, uAxis) + u * s) / (2.0f * s)));
				const int cv = static_cast<int>(std::floor(
					(axisValue(config.sampleOrigin, vAxis) + v * s) / (2.0f * s)));
				coarseToFine[{cu, cv}].push_back({u, v});
			}
		}

		for (const auto& pair : coarseToFine)
		{
			const auto& fineCells = pair.second;
			if (fineCells.empty())
			{
				continue;
			}

			// Use the first fine cell to represent this coarse cell for target calculation.
			const int representativeU = fineCells.front().first;
			const int representativeV = fineCells.front().second;
			int boundaryLocalCell[3] = { 0, 0, 0 };
			boundaryLocalCell[axis] = faceLocal;
			boundaryLocalCell[uAxis] = representativeU;
			boundaryLocalCell[vAxis] = representativeV;
			const DualVertexResult coarseVertex = calculateCoarseDualVertexForFineCell(
				config, boundaryLocalCell);
			if (!coarseVertex.exists)
			{
				continue;
			}

			// Collect interior dual vertices for the fine cells in this coarse cell,
			// then sort them into cyclic order around the coarse dual vertex.
			int minU = fineCells.front().first;
			int maxU = fineCells.front().first;
			int minV = fineCells.front().second;
			int maxV = fineCells.front().second;
			for (const auto& fineCell : fineCells)
			{
				minU = std::min(minU, fineCell.first);
				maxU = std::max(maxU, fineCell.first);
				minV = std::min(minV, fineCell.second);
				maxV = std::max(maxV, fineCell.second);
			}

			std::array<std::pair<int, int>, 4> cyclicOrder = {{
				{minU, minV},
				{maxU, minV},
				{maxU, maxV},
				{minU, maxV}
			}};

			std::vector<int> interiorIndices;
			interiorIndices.reserve(4);
			for (const auto& coord : cyclicOrder)
			{
				int dualLocalCell[3] = { 0, 0, 0 };
				dualLocalCell[axis] = faceLocal;
				dualLocalCell[uAxis] = coord.first;
				dualLocalCell[vAxis] = coord.second;
				const int index = cellVertexIndex[getIndex(
					dualLocalCell[0], dualLocalCell[1], dualLocalCell[2], voxelSize)];
				if (index >= 0 && std::find(interiorIndices.begin(), interiorIndices.end(), index) == interiorIndices.end())
				{
					interiorIndices.push_back(index);
				}
			}

			if (interiorIndices.size() < 2)
			{
				continue;
			}

			int materialLocalCell[3] = { 0, 0, 0 };
			materialLocalCell[axis] = materialLocal;
			materialLocalCell[uAxis] = representativeU;
			materialLocalCell[vAxis] = representativeV;
			const voxelInfo material = sampleGlobalVoxelAtLocalCell(config, materialLocalCell);
			const int boundaryIndex = addStitchedVertex(mesh, coarseVertex.worldPosition, material);
			if (interiorIndices.size() == 2)
			{
				addTransitionTriangleIfValid(mesh, interiorIndices[0], boundaryIndex, interiorIndices[1], config);
				continue;
			}

			// Emit fan triangles and orient them against the local density gradient
			// so terrain back-face culling does not hide generated transition strips.
			for (size_t i = 0; i < interiorIndices.size(); ++i)
			{
				const int prev = interiorIndices[i];
				const int cur = interiorIndices[(i + 1) % interiorIndices.size()];
				addTransitionTriangleIfValid(mesh, prev, boundaryIndex, cur, config);
			}
		}
	}

} // namespace

SurfaceNets::SurfaceNets()
{
}

void SurfaceNets::generate(vec3 basePoint, Mesh* mesh, const voxelInfo* srcData,
	const SurfaceNetsGenerateConfig& config)
{
	if (!mesh || !srcData)
	{
		return;
	}

	const int voxelSize = config.voxelSize;
	const int cellCount = config.cellCount;
	const int minCell = config.minPadding;
	const int maxCell = minCell + cellCount;
	const int minCellExt = minCell - 1;
	const float cellWorldSize = config.cellWorldSize;
	const unsigned char isoLevel = config.isoLevel;
	if (voxelSize <= 0 || cellCount <= 0 || minCell <= 0 || cellWorldSize <= 0.0f
		|| minCellExt < 0 || maxCell >= voxelSize)
	{
		return;
	}
	std::vector<int> cellVertexIndex(voxelSize * voxelSize * voxelSize, -1);

	// ---- Pass 1: dual vertex generation ------------------------------------
	// Negative-side halo cells are generated so this chunk can own the seam
	// on its -X/-Y/-Z boundaries. Positive-side boundary faces are skipped in
	// Pass 2, so adjacent chunks do not emit the same boundary twice.
	for (int z = minCellExt; z < maxCell; ++z)
	{
		for (int y = minCellExt; y < maxCell; ++y)
		{
			for (int x = minCellExt; x < maxCell; ++x)
			{
				const DualVertexResult vertex = calculateDualVertex(
					basePoint, srcData, voxelSize, minCell, cellWorldSize, isoLevel, x, y, z);
				if (!vertex.exists)
				{
					continue;
				}

				mesh->addVertex(makeVertexData(vertex.worldPosition, vertex.material));
				cellVertexIndex[getIndex(x, y, z, voxelSize)] = static_cast<int>(mesh->getVerticesSize()) - 1;
			}
		}
	}

	// ---- Pass 2: triangle generation ---------------------------------------
	for (int z = minCellExt; z <= maxCell; ++z)
	{
		for (int y = minCellExt; y <= maxCell; ++y)
		{
			for (int x = minCellExt; x <= maxCell; ++x)
			{
				const unsigned char valSelf = getValue(srcData, voxelSize, x, y, z);
				const bool selfIn = valSelf < isoLevel;

				if (x + 1 < voxelSize && y >= minCell && z >= minCell
					&& x != maxCell - 1)
				{
					const unsigned char valX = getValue(srcData, voxelSize, x + 1, y, z);
					if (selfIn != (valX < isoLevel))
					{
						// NegativeX boundary quads are emitted at x == minCell - 1.
						// Suppress them when the finer neighbor owns LOD transition geometry
						// or when this node must not overlap a finer neighbor's strip.
						const bool isNegativeBoundary = (x == minCell - 1);
						const TerrainMeshSeamMode mode = config.seams.faces[0].mode;
						const bool suppress = isNegativeBoundary
							&& (mode == TerrainMeshSeamMode::StitchToCoarser
								|| mode == TerrainMeshSeamMode::SuppressForFiner);
						if (!suppress)
						{
							const int c1 = cellVertexIndex[getIndex(x, y - 1, z - 1, voxelSize)];
							const int c2 = cellVertexIndex[getIndex(x, y, z - 1, voxelSize)];
							const int c3 = cellVertexIndex[getIndex(x, y, z, voxelSize)];
							const int c4 = cellVertexIndex[getIndex(x, y - 1, z, voxelSize)];
							addQuad(mesh, c1, c2, c3, c4, !selfIn);
						}
					}
				}

				if (y + 1 < voxelSize && x >= minCell && z >= minCell
					&& y != maxCell - 1)
				{
					const unsigned char valY = getValue(srcData, voxelSize, x, y + 1, z);
					if (selfIn != (valY < isoLevel))
					{
						const bool isNegativeBoundary = (y == minCell - 1);
						const TerrainMeshSeamMode mode = config.seams.faces[2].mode;
						const bool suppress = isNegativeBoundary
							&& (mode == TerrainMeshSeamMode::StitchToCoarser
								|| mode == TerrainMeshSeamMode::SuppressForFiner);
						if (!suppress)
						{
							const int c1 = cellVertexIndex[getIndex(x - 1, y, z - 1, voxelSize)];
							const int c2 = cellVertexIndex[getIndex(x, y, z - 1, voxelSize)];
							const int c3 = cellVertexIndex[getIndex(x, y, z, voxelSize)];
							const int c4 = cellVertexIndex[getIndex(x - 1, y, z, voxelSize)];
							addQuad(mesh, c1, c2, c3, c4, selfIn);
						}
					}
				}

				if (z + 1 < voxelSize && x >= minCell && y >= minCell
					&& z != maxCell - 1)
				{
					const unsigned char valZ = getValue(srcData, voxelSize, x, y, z + 1);
					if (selfIn != (valZ < isoLevel))
					{
						const bool isNegativeBoundary = (z == minCell - 1);
						const TerrainMeshSeamMode mode = config.seams.faces[4].mode;
						const bool suppress = isNegativeBoundary
							&& (mode == TerrainMeshSeamMode::StitchToCoarser
								|| mode == TerrainMeshSeamMode::SuppressForFiner);
						if (!suppress)
						{
							const int c1 = cellVertexIndex[getIndex(x - 1, y - 1, z, voxelSize)];
							const int c2 = cellVertexIndex[getIndex(x, y - 1, z, voxelSize)];
							const int c3 = cellVertexIndex[getIndex(x, y, z, voxelSize)];
							const int c4 = cellVertexIndex[getIndex(x - 1, y, z, voxelSize)];
							addQuad(mesh, c1, c2, c3, c4, !selfIn);
						}
					}
				}
			}
		}
	}

	// ---- Pass 3: LOD transition strips -------------------------------------
	// Emit fine-side transition strips for faces adjacent to a coarser neighbor.
	for (int face = 0; face < 6; ++face)
	{
		if (config.seams.faces[face].mode == TerrainMeshSeamMode::StitchToCoarser)
		{
			emitTransitionStripForFace(mesh, config, cellVertexIndex, face);
		}
	}

	mesh->caclNormals();
}

} // namespace tzw
