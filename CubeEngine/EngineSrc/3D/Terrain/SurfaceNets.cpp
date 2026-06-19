#include "SurfaceNets.h"

#include "GameMap.h"
#include <algorithm>
#include <array>
#include <cmath>
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

	vec4 debugLodColor(int lodPower)
	{
		switch (lodPower)
		{
		case 0:
			return vec4(0, 1, 0, 1);
		case 1:
			return vec4(1, 0, 0, 1);
		case 2:
			return vec4(0, 0, 1, 1);
		case 3:
			return vec4(1, 0, 1, 1);
		case 4:
			return vec4(0, 1, 1, 1);
		default:
			return vec4(1, 1, 1, 1);
		}
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

	bool isStitchFace(const SurfaceNetsGenerateConfig& config, int faceIndex)
	{
		return config.seams.faces[faceIndex].mode == TerrainMeshSeamMode::StitchToCoarser;
	}

	bool edgeTouchesFace(int edgeAxis, const int cell[3], int faceIndex, int minCell, int maxCell)
	{
		const FaceInfo face = getFaceInfo(faceIndex);
		if (edgeAxis == face.axis)
		{
			return face.side < 0
				? cell[face.axis] == minCell - 1
				: cell[face.axis] == maxCell - 1;
		}
		return face.side < 0
			? cell[face.axis] == minCell
			: cell[face.axis] == maxCell;
	}

	bool rangesOverlap(int minA, int maxA, int minB, int maxB)
	{
		return minA < maxB && minB < maxA;
	}

	struct FaceAxisRange
	{
		int minValue = 0;
		int maxValue = 0;
		bool isPoint = false;
	};

	void edgeFaceRange(int edgeAxis, const int cell[3], int tangentAxis,
		FaceAxisRange& range)
	{
		if (edgeAxis == tangentAxis)
		{
			range.minValue = cell[tangentAxis];
			range.maxValue = cell[tangentAxis] + 1;
			range.isPoint = false;
		}
		else
		{
			range.minValue = cell[tangentAxis];
			range.maxValue = cell[tangentAxis];
			range.isPoint = true;
		}
	}

	uint8_t halfMaskForRange(const FaceAxisRange& range, int minCell, int maxCell)
	{
		const int mid = (minCell + maxCell) / 2;
		if (range.isPoint)
		{
			const int value = std::clamp(range.minValue, minCell, maxCell);
			if (value == mid)
			{
				return 0x3;
			}
			return value < mid ? 0x1 : 0x2;
		}

		int rangeMin = std::max(range.minValue, minCell);
		int rangeMax = std::min(range.maxValue, maxCell);
		if (rangeMin >= rangeMax)
		{
			const int value = std::clamp(range.minValue, minCell, maxCell);
			return value < mid ? 0x1 : 0x2;
		}

		uint8_t mask = 0;
		if (rangesOverlap(rangeMin, rangeMax, minCell, mid))
		{
			mask |= 0x1;
		}
		if (rangesOverlap(rangeMin, rangeMax, mid, maxCell))
		{
			mask |= 0x2;
		}
		return mask;
	}

	uint8_t edgeCoverageMaskOnFace(int edgeAxis, const int cell[3],
		int faceIndex, int minCell, int maxCell)
	{
		const FaceInfo face = getFaceInfo(faceIndex);
		FaceAxisRange range0;
		FaceAxisRange range1;
		edgeFaceRange(edgeAxis, cell, face.tangential[0], range0);
		edgeFaceRange(edgeAxis, cell, face.tangential[1], range1);
		const uint8_t mask0 = halfMaskForRange(range0, minCell, maxCell);
		const uint8_t mask1 = halfMaskForRange(range1, minCell, maxCell);
		uint8_t mask = 0;
		for (int half1 = 0; half1 < 2; ++half1)
		{
			if ((mask1 & (1u << half1)) == 0)
			{
				continue;
			}
			for (int half0 = 0; half0 < 2; ++half0)
			{
				if ((mask0 & (1u << half0)) != 0)
				{
					mask |= static_cast<uint8_t>(1u << (half0 + half1 * 2));
				}
			}
		}
		return mask;
	}

	bool shouldSuppressEdgeForFiner(const SurfaceNetsGenerateConfig& config,
		int edgeAxis, const int cell[3])
	{
		const int minCell = config.minPadding;
		const int maxCell = minCell + config.cellCount;
		for (int faceIndex = 0; faceIndex < 6; ++faceIndex)
		{
			const FaceInfo face = getFaceInfo(faceIndex);
			if (edgeAxis == face.axis)
			{
				continue;
			}
			if (config.seams.faces[faceIndex].mode != TerrainMeshSeamMode::SuppressForFiner)
			{
				continue;
			}
			if (!edgeTouchesFace(edgeAxis, cell, faceIndex, minCell, maxCell))
			{
				continue;
			}
			const uint8_t coverageMask = config.seams.faces[faceIndex].finerCoverageMask;
			const uint8_t edgeMask = edgeCoverageMaskOnFace(edgeAxis, cell, faceIndex, minCell, maxCell);
			if ((coverageMask & edgeMask) != 0)
			{
				return true;
			}
		}
		return false;
	}

	bool positiveAxisEdgeOwnedByNeighbor(const SurfaceNetsGenerateConfig& config, int axis)
	{
		return config.seams.faces[axis * 2 + 1].mode == TerrainMeshSeamMode::SameLevel;
	}

	bool edgeTouchesStitchFace(const SurfaceNetsGenerateConfig& config,
		int edgeAxis, const int cell[3])
	{
		const int minCell = config.minPadding;
		const int maxCell = minCell + config.cellCount;
		for (int faceIndex = 0; faceIndex < 6; ++faceIndex)
		{
			if (!isStitchFace(config, faceIndex))
			{
				continue;
			}
			if (edgeTouchesFace(edgeAxis, cell, faceIndex, minCell, maxCell))
			{
				return true;
			}
		}
		return false;
	}

	bool isOnStitchFace(const SurfaceNetsGenerateConfig& config, const int cell[3])
	{
		const int minCellExt = config.minPadding - 1;
		const int maxCell = config.minPadding + config.cellCount;
		for (int axis = 0; axis < 3; ++axis)
		{
			if (cell[axis] == minCellExt && isStitchFace(config, axis * 2))
			{
				return true;
			}
			if (cell[axis] == maxCell && isStitchFace(config, axis * 2 + 1))
			{
				return true;
			}
		}
		return false;
	}

	uint8_t stitchPreserveFineBorderBit(int tangentIndex, int side)
	{
		return static_cast<uint8_t>(1u << (tangentIndex * 2 + (side > 0 ? 1 : 0)));
	}

	bool shouldSnapStitchCell(const SurfaceNetsGenerateConfig& config,
		int faceIndex, const FaceInfo& face, const int cell[3])
	{
		const int minCell = config.minPadding;
		const int minCellExt = minCell - 1;
		const int maxCell = minCell + config.cellCount;
		const uint8_t preserveMask = config.seams.faces[faceIndex].stitchPreserveFineBorderMask;
		for (int i = 0; i < 2; ++i)
		{
			const int tangentAxis = face.tangential[i];
			if ((cell[tangentAxis] == minCellExt || cell[tangentAxis] == minCell)
				&& (preserveMask & stitchPreserveFineBorderBit(i, -1)) != 0)
			{
				return false;
			}
			if ((cell[tangentAxis] == maxCell - 1 || cell[tangentAxis] == maxCell)
				&& (preserveMask & stitchPreserveFineBorderBit(i, +1)) != 0)
			{
				return false;
			}
		}
		return true;
	}

	bool shouldGenerateCellVertex(const SurfaceNetsGenerateConfig& config, int x, int y, int z)
	{
		const int minCell = config.minPadding;
		const int maxCell = minCell + config.cellCount;
		const int minCellExt = minCell - 1;
		const int cell[3] = { x, y, z };
		for (int axis = 0; axis < 3; ++axis)
		{
			if (cell[axis] < minCellExt || cell[axis] > maxCell)
			{
				return false;
			}
		}
		if (isOnStitchFace(config, cell))
		{
			return true;
		}
		for (int axis = 0; axis < 3; ++axis)
		{
			if (cell[axis] == maxCell && !isStitchFace(config, axis * 2 + 1))
			{
				return false;
			}
		}
		return true;
	}

	bool shouldEmitSurfaceEdge(const SurfaceNetsGenerateConfig& config, int axis, int x, int y, int z)
	{
		const int minCell = config.minPadding;
		const int maxCell = minCell + config.cellCount;
		const int minCellExt = minCell - 1;
		const int cell[3] = { x, y, z };
		if (cell[axis] < minCellExt || cell[axis] > maxCell - 1)
		{
			return false;
		}
		if (shouldSuppressEdgeForFiner(config, axis, cell))
		{
			return false;
		}
		if (cell[axis] == maxCell - 1 && positiveAxisEdgeOwnedByNeighbor(config, axis))
		{
			return false;
		}
		const bool touchesStitchFace = edgeTouchesStitchFace(config, axis, cell);

		for (int tangentAxis = 0; tangentAxis < 3; ++tangentAxis)
		{
			if (tangentAxis == axis)
			{
				continue;
			}
			if (cell[tangentAxis] < minCell || cell[tangentAxis] > maxCell)
			{
				return false;
			}
			if (cell[tangentAxis] == maxCell
				&& !isStitchFace(config, tangentAxis * 2 + 1)
				&& !touchesStitchFace)
			{
				return false;
			}
		}
		return true;
	}

	void snapStitchedFaceLayer(Mesh* mesh, const SurfaceNetsGenerateConfig& config,
		std::vector<int>& cellVertexIndex, int faceIndex, int faceLocal)
	{
		const FaceInfo face = getFaceInfo(faceIndex);
		const int minCell = config.minPadding;
		const int maxCell = minCell + config.cellCount;
		const int minCellExt = minCell - 1;
		const int voxelSize = config.voxelSize;

		for (int u = minCellExt; u <= maxCell; ++u)
		{
			for (int v = minCellExt; v <= maxCell; ++v)
			{
				int localCell[3] = { 0, 0, 0 };
				localCell[face.axis] = faceLocal;
				localCell[face.tangential[0]] = u;
				localCell[face.tangential[1]] = v;
				if (!shouldSnapStitchCell(config, faceIndex, face, localCell))
				{
					continue;
				}
				const int index = cellVertexIndex[getIndex(
					localCell[0], localCell[1], localCell[2], voxelSize)];
				if (index < 0)
				{
					continue;
				}
				const DualVertexResult coarseVertex = calculateCoarseDualVertexForFineCell(
					config, localCell);
				if (!coarseVertex.exists)
				{
					continue;
				}
				mesh->m_vertices[index].m_pos = coarseVertex.worldPosition;
				mesh->m_vertices[index].m_texCoord = vec2(
					coarseVertex.worldPosition.x,
					coarseVertex.worldPosition.z);
				if (config.debugLodVertexColorEnabled)
				{
					mesh->m_vertices[index].m_color = vec4(1, 1, 0, 1);
				}
			}
		}
	}

	void snapStitchedFaceVertices(Mesh* mesh, const SurfaceNetsGenerateConfig& config,
		std::vector<int>& cellVertexIndex, int faceIndex)
	{
		const FaceInfo face = getFaceInfo(faceIndex);
		const int minCell = config.minPadding;
		const int maxCell = minCell + config.cellCount;
		if (face.side < 0)
		{
			snapStitchedFaceLayer(mesh, config, cellVertexIndex, faceIndex, minCell - 1);
			snapStitchedFaceLayer(mesh, config, cellVertexIndex, faceIndex, minCell);
		}
		else
		{
			snapStitchedFaceLayer(mesh, config, cellVertexIndex, faceIndex, maxCell - 1);
			snapStitchedFaceLayer(mesh, config, cellVertexIndex, faceIndex, maxCell);
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
		|| minCellExt < 0 || maxCell + 1 >= voxelSize)
	{
		return;
	}
	std::vector<int> cellVertexIndex(voxelSize * voxelSize * voxelSize, -1);

	// ---- Pass 1: dual vertex generation ------------------------------------
	// Negative-side halo cells are generated so this chunk can own the seam
	// on its -X/-Y/-Z boundaries. Positive-side halo cells are generated only
	// for fine-to-coarse stitch faces, where this fine chunk owns the transition.
	for (int z = minCellExt; z <= maxCell; ++z)
	{
		for (int y = minCellExt; y <= maxCell; ++y)
		{
			for (int x = minCellExt; x <= maxCell; ++x)
			{
				if (!shouldGenerateCellVertex(config, x, y, z))
				{
					continue;
				}
				const DualVertexResult vertex = calculateDualVertex(
					basePoint, srcData, voxelSize, minCell, cellWorldSize, isoLevel, x, y, z);
				if (!vertex.exists)
				{
					continue;
				}

				VertexData vertexData = makeVertexData(vertex.worldPosition, vertex.material);
				if (config.debugLodVertexColorEnabled)
				{
					vertexData.m_color = debugLodColor(config.debugLodPower);
				}
				mesh->addVertex(vertexData);
				cellVertexIndex[getIndex(x, y, z, voxelSize)] = static_cast<int>(mesh->getVerticesSize()) - 1;
			}
		}
	}

	for (int face = 0; face < 6; ++face)
	{
		if (isStitchFace(config, face))
		{
			snapStitchedFaceVertices(mesh, config, cellVertexIndex, face);
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

				if (x + 1 < voxelSize && shouldEmitSurfaceEdge(config, 0, x, y, z))
				{
					const unsigned char valX = getValue(srcData, voxelSize, x + 1, y, z);
					if (selfIn != (valX < isoLevel))
					{
						const int c1 = cellVertexIndex[getIndex(x, y - 1, z - 1, voxelSize)];
						const int c2 = cellVertexIndex[getIndex(x, y, z - 1, voxelSize)];
						const int c3 = cellVertexIndex[getIndex(x, y, z, voxelSize)];
						const int c4 = cellVertexIndex[getIndex(x, y - 1, z, voxelSize)];
						addQuad(mesh, c1, c2, c3, c4, !selfIn);
					}
				}

				if (y + 1 < voxelSize && shouldEmitSurfaceEdge(config, 1, x, y, z))
				{
					const unsigned char valY = getValue(srcData, voxelSize, x, y + 1, z);
					if (selfIn != (valY < isoLevel))
					{
						const int c1 = cellVertexIndex[getIndex(x - 1, y, z - 1, voxelSize)];
						const int c2 = cellVertexIndex[getIndex(x, y, z - 1, voxelSize)];
						const int c3 = cellVertexIndex[getIndex(x, y, z, voxelSize)];
						const int c4 = cellVertexIndex[getIndex(x - 1, y, z, voxelSize)];
						addQuad(mesh, c1, c2, c3, c4, selfIn);
					}
				}

				if (z + 1 < voxelSize && shouldEmitSurfaceEdge(config, 2, x, y, z))
				{
					const unsigned char valZ = getValue(srcData, voxelSize, x, y, z + 1);
					if (selfIn != (valZ < isoLevel))
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

	mesh->caclNormals();
}

} // namespace tzw
