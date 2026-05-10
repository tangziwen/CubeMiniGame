#include "SurfaceNets.h"

#include "CubeGame/GameConfig.h"
#include "CubeGame/GameMap.h"
#include "TransVoxel.h"
#include "Utility/log/Log.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace tzw {

namespace {
	constexpr int ISOLEVEL = 128;

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

	voxelInfo* getVoxel(voxelInfo* srcData, int voxelSize, int x, int y, int z)
	{
		return srcData + getIndex(x, y, z, voxelSize);
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

	// Compute the dual-vertex position (in the same world space as `basePoint`) of
	// the cell at integer coordinates (x, y, z) inside an arbitrary LOD voxel grid.
	// Returns false when the cell does not straddle the iso-surface or the indices
	// fall outside the buffer.
	bool computeDualVertexPosition(vec3 basePoint, int voxelSize, voxelInfo* srcData,
		int lodLevel, int x, int y, int z, vec3& outPosition)
	{
		if (!srcData) return false;
		if (x < 0 || y < 0 || z < 0) return false;
		if (x + 1 >= voxelSize || y + 1 >= voxelSize || z + 1 >= voxelSize) return false;

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
			if (corners[i] < ISOLEVEL) mask |= (1 << i);
		}
		if (mask == 0 || mask == 0xff) return false;

		vec3 sum(0, 0, 0);
		int count = 0;
		for (const auto& edge : edgePairs())
		{
			const int dx1 = edge[0][0], dy1 = edge[0][1], dz1 = edge[0][2];
			const int dx2 = edge[1][0], dy2 = edge[1][1], dz2 = edge[1][2];
			const unsigned char v1 = getValue(srcData, voxelSize, x + dx1, y + dy1, z + dz1);
			const unsigned char v2 = getValue(srcData, voxelSize, x + dx2, y + dy2, z + dz2);
			if ((v1 < ISOLEVEL) != (v2 < ISOLEVEL))
			{
				const vec3 p1(static_cast<float>(x + dx1), static_cast<float>(y + dy1), static_cast<float>(z + dz1));
				const vec3 p2(static_cast<float>(x + dx2), static_cast<float>(y + dy2), static_cast<float>(z + dz2));
				sum += interpolate(p1, v1, p2, v2);
				++count;
			}
		}
		if (count == 0) return false;

		const int lodScale = 1 << lodLevel;
		const vec3 center = sum / static_cast<float>(count);
		outPosition = (center - vec3(static_cast<float>(MIN_PADDING))) * static_cast<float>(lodScale) * BLOCK_SIZE + basePoint;
		return true;
	}
}

SurfaceNets::SurfaceNets()
{
}

void SurfaceNets::generate(vec3 basePoint, Mesh* mesh, int voxelSize, voxelInfo* srcData,
	float minValue, int lodLevel, const SurfaceNetsStitchConfig* stitchConfig)
{
	(void)minValue;
	if (!mesh || !srcData)
	{
		return;
	}

	const int lodScale = 1 << lodLevel;
	const float lodScaleF = static_cast<float>(lodScale);
	const int cellCount = MAX_BLOCK >> lodLevel;
	const int minCell = MIN_PADDING;
	const int maxCell = MIN_PADDING + cellCount;
	// To avoid holes between same-LOD neighbour chunks we additionally produce
	// dual vertices for one row of cells in the -X/-Y/-Z padding (cell index
	// MIN_PADDING - 1). Those padding cells cover the same global voxel volume
	// as the +side neighbour chunk's first interior cell, so positions match
	// exactly. Pass 2 below uses these padding-row vertices to close the
	// -side boundary quad. The +side chunk does *not* fill its +side padding
	// cells, ensuring each boundary quad is generated exactly once (by the
	// chunk that owns the -side of the boundary).
	//
	// Exception: when stitchConfig->extendPositive[axis] is set, this chunk is
	// the coarser side of an LOD transition along that axis. The finer +side
	// neighbour collapses its -side seam quad, so we must draw the seam from
	// here. For each such axis we additionally fill one row of +side padding
	// cells (cell index maxCell), letting Pass 2 emit the +side boundary quad.
	const int minCellExt = minCell - 1;
	const int xMaxCell = maxCell + ((stitchConfig && stitchConfig->extendPositive[0]) ? 1 : 0);
	const int yMaxCell = maxCell + ((stitchConfig && stitchConfig->extendPositive[1]) ? 1 : 0);
	const int zMaxCell = maxCell + ((stitchConfig && stitchConfig->extendPositive[2]) ? 1 : 0);
	std::vector<int> cellVertexIndex(voxelSize * voxelSize * voxelSize, -1);

	// ---- Pass 1: dual vertex generation ------------------------------------
	for (int z = minCellExt; z < zMaxCell; ++z)
	{
		for (int y = minCellExt; y < yMaxCell; ++y)
		{
			for (int x = minCellExt; x < xMaxCell; ++x)
			{
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
					if (corners[i] < ISOLEVEL) mask |= (1 << i);
				}
				if (mask == 0 || mask == 0xff) continue;

				vec3 sum(0, 0, 0);
				int count = 0;
				for (const auto& edge : edgePairs())
				{
					const int dx1 = edge[0][0], dy1 = edge[0][1], dz1 = edge[0][2];
					const int dx2 = edge[1][0], dy2 = edge[1][1], dz2 = edge[1][2];
					const unsigned char v1 = getValue(srcData, voxelSize, x + dx1, y + dy1, z + dz1);
					const unsigned char v2 = getValue(srcData, voxelSize, x + dx2, y + dy2, z + dz2);
					if ((v1 < ISOLEVEL) != (v2 < ISOLEVEL))
					{
						const vec3 p1(static_cast<float>(x + dx1), static_cast<float>(y + dy1), static_cast<float>(z + dz1));
						const vec3 p2(static_cast<float>(x + dx2), static_cast<float>(y + dy2), static_cast<float>(z + dz2));
						sum += interpolate(p1, v1, p2, v2);
						++count;
					}
				}
				if (count == 0) continue;

				const vec3 center = sum / static_cast<float>(count);
				const vec3 finalPosition = (center - vec3(static_cast<float>(MIN_PADDING))) * lodScaleF * BLOCK_SIZE + basePoint;
				const int matX = std::clamp(static_cast<int>(center.x), 0, voxelSize - 1);
				const int matY = std::clamp(static_cast<int>(center.y), 0, voxelSize - 1);
				const int matZ = std::clamp(static_cast<int>(center.z), 0, voxelSize - 1);
				mesh->addVertex(makeVertexData(finalPosition, *getVoxel(srcData, voxelSize, matX, matY, matZ)));
				cellVertexIndex[getIndex(x, y, z, voxelSize)] = static_cast<int>(mesh->getVerticesSize()) - 1;
			}
		}
	}

	// ---- Pass 2: triangle generation ---------------------------------------
	// Outer loops start at minCellExt so that the boundary quad for each axis
	// (the face between voxel index 0 / padding and voxel index 1 / first
	// interior) is visited. Without this, e.g. the X-perpendicular boundary
	// quad at x = minCellExt is never generated and -X seams stay open. The
	// per-axis inner conditions (y >= minCell, z >= minCell, ...) ensure the
	// other two axes remain inside the buffer and don't double-emit anything.
	//
	// On a stitched -X / -Y / -Z face we must NOT emit the corresponding
	// boundary quad: the coarser neighbour owns that seam (its extendPositive
	// path draws it). Previously the Pass 3 snap collapsed those quads into
	// degenerate triangles "by accident" because in-plane truncation toward
	// zero forced all 4 corners onto the same coarse cell. With the floor
	// mapping required for correct corner alignment that incidental collapse
	// is gone, so we now skip explicitly.
	const bool skipNegXQuad = stitchConfig && stitchConfig->stitchFace[0];
	const bool skipNegYQuad = stitchConfig && stitchConfig->stitchFace[2];
	const bool skipNegZQuad = stitchConfig && stitchConfig->stitchFace[4];
	// Same-LOD convention: the +side neighbour chunk owns the boundary quad
	// (it draws it from its -side padding row at x|y|z == minCellExt). The
	// -side chunk would otherwise also emit the same boundary quad from its
	// last interior column (x|y|z == maxCell - 1), producing a duplicated /
	// overlapping strip exactly at the chunk seam.
	//
	// Whether to actually skip is decided by the caller via `skipPositive`,
	// which is only set to true when the +side neighbour really exists and
	// will draw the boundary itself. Without this guard, world-edge chunks
	// or chunks whose +side neighbour hasn't streamed in yet would leave the
	// boundary completely undrawn.
	const bool skipPosXQuad = stitchConfig && stitchConfig->skipPositive[0];
	const bool skipPosYQuad = stitchConfig && stitchConfig->skipPositive[1];
	const bool skipPosZQuad = stitchConfig && stitchConfig->skipPositive[2];
	// When extendPositive[axis] is set, Pass 1 fills one extra row of dual
	// vertices at axis-coord == maxCell so that the *seam* quad at
	// axis-coord == maxCell - 1 can be emitted from this (coarser) side.
	// However, the Pass 2 outer loop also visits axis-coord == maxCell, and at
	// that index a spurious quad would be emitted whose active edge sits at
	// axis-coord == maxCell + 1 -- i.e. one full coarse stride INTO the +side
	// neighbour's territory. The +side (finer) neighbour already draws its own
	// geometry there, producing a near-coplanar overlap strip that becomes
	// visible whenever the camera moves through an LOD transition.
	// Suppress the spurious emit explicitly. Without this the previous
	// skipPositive fix only cleared the same-LOD overlap; the cross-LOD
	// (extendPositive) overlap survived. See SurfaceNetsLod.md §3.7.
	const bool extPosX = stitchConfig && stitchConfig->extendPositive[0];
	const bool extPosY = stitchConfig && stitchConfig->extendPositive[1];
	const bool extPosZ = stitchConfig && stitchConfig->extendPositive[2];
	for (int z = minCellExt; z <= maxCell; ++z)
	{
		for (int y = minCellExt; y <= maxCell; ++y)
		{
			for (int x = minCellExt; x <= maxCell; ++x)
			{
				const unsigned char valSelf = getValue(srcData, voxelSize, x, y, z);
				const bool selfIn = valSelf < ISOLEVEL;

				if (x + 1 < voxelSize && y >= minCell && z >= minCell
					&& !(skipNegXQuad && x == minCellExt)
					&& !(skipPosXQuad && x == maxCell - 1)
					&& !(extPosX && x == maxCell))
				{
					const unsigned char valX = getValue(srcData, voxelSize, x + 1, y, z);
					if (selfIn != (valX < ISOLEVEL))
					{
						const int c1 = cellVertexIndex[getIndex(x, y - 1, z - 1, voxelSize)];
						const int c2 = cellVertexIndex[getIndex(x, y, z - 1, voxelSize)];
						const int c3 = cellVertexIndex[getIndex(x, y, z, voxelSize)];
						const int c4 = cellVertexIndex[getIndex(x, y - 1, z, voxelSize)];
						if (c1 != -1 && c2 != -1 && c3 != -1 && c4 != -1)
						{
							if (selfIn)
							{
								mesh->addIndex(c1); mesh->addIndex(c2); mesh->addIndex(c3);
								mesh->addIndex(c1); mesh->addIndex(c3); mesh->addIndex(c4);
							}
							else
							{
								mesh->addIndex(c1); mesh->addIndex(c3); mesh->addIndex(c2);
								mesh->addIndex(c1); mesh->addIndex(c4); mesh->addIndex(c3);
							}
						}
					}
				}

				if (y + 1 < voxelSize && x >= minCell && z >= minCell
					&& !(skipNegYQuad && y == minCellExt)
					&& !(skipPosYQuad && y == maxCell - 1)
					&& !(extPosY && y == maxCell))
				{
					const unsigned char valY = getValue(srcData, voxelSize, x, y + 1, z);
					if (selfIn != (valY < ISOLEVEL))
					{
						const int c1 = cellVertexIndex[getIndex(x - 1, y, z - 1, voxelSize)];
						const int c2 = cellVertexIndex[getIndex(x, y, z - 1, voxelSize)];
						const int c3 = cellVertexIndex[getIndex(x, y, z, voxelSize)];
						const int c4 = cellVertexIndex[getIndex(x - 1, y, z, voxelSize)];
						if (c1 != -1 && c2 != -1 && c3 != -1 && c4 != -1)
						{
							if (selfIn)
							{
								mesh->addIndex(c1); mesh->addIndex(c3); mesh->addIndex(c2);
								mesh->addIndex(c1); mesh->addIndex(c4); mesh->addIndex(c3);
							}
							else
							{
								mesh->addIndex(c1); mesh->addIndex(c2); mesh->addIndex(c3);
								mesh->addIndex(c1); mesh->addIndex(c3); mesh->addIndex(c4);
							}
						}
					}
				}

				if (z + 1 < voxelSize && x >= minCell && y >= minCell
					&& !(skipNegZQuad && z == minCellExt)
					&& !(skipPosZQuad && z == maxCell - 1)
					&& !(extPosZ && z == maxCell))
				{
					const unsigned char valZ = getValue(srcData, voxelSize, x, y, z + 1);
					if (selfIn != (valZ < ISOLEVEL))
					{
						const int c1 = cellVertexIndex[getIndex(x - 1, y - 1, z, voxelSize)];
						const int c2 = cellVertexIndex[getIndex(x, y - 1, z, voxelSize)];
						const int c3 = cellVertexIndex[getIndex(x, y, z, voxelSize)];
						const int c4 = cellVertexIndex[getIndex(x - 1, y, z, voxelSize)];
						if (c1 != -1 && c2 != -1 && c3 != -1 && c4 != -1)
						{
							if (selfIn)
							{
								mesh->addIndex(c1); mesh->addIndex(c2); mesh->addIndex(c3);
								mesh->addIndex(c1); mesh->addIndex(c3); mesh->addIndex(c4);
							}
							else
							{
								mesh->addIndex(c1); mesh->addIndex(c3); mesh->addIndex(c2);
								mesh->addIndex(c1); mesh->addIndex(c4); mesh->addIndex(c3);
							}
						}
					}
				}
			}
		}
	}

	// ---- Pass 3: vertex stitching for LOD seams ----------------------------
	// For each fine cell that lies on a face flagged for stitching, snap its dual
	// vertex onto the dual-vertex position of the coarse cell on the *neighbour*
	// side of that face. The coarse buffer is sampled from the same global anchor
	// as the fine buffer with stride = 2 * fine stride, so its padding cells
	// cover the first row of voxels that belong to the coarser neighbour chunk:
	//   -X face : cx = MIN_PADDING - 1                     (= 0)
	//   +X face : cx = MIN_PADDING + (MAX_BLOCK >> coarseLod)
	// (and analogously for Y / Z). Snapping the boundary fine vertex to that
	// coarse-cell dual vertex matches exactly the vertex the neighbour coarse
	// chunk produces for its first interior cell — yielding a watertight join.
	// Multiple fine cells sharing the same coarse target collapse to one point,
	// producing degenerate (zero-area) triangles that vanish at render time.
	if (stitchConfig && stitchConfig->coarseData && stitchConfig->coarseVoxelSize > 0)
	{
		const bool anyFace =
			stitchConfig->stitchFace[0] || stitchConfig->stitchFace[1] ||
			stitchConfig->stitchFace[2] || stitchConfig->stitchFace[3] ||
			stitchConfig->stitchFace[4] || stitchConfig->stitchFace[5];
		if (anyFace)
		{
			const int boundaryMin = minCell;
			const int boundaryMax = maxCell - 1;
			const int coarseCells = MAX_BLOCK >> stitchConfig->coarseLodLevel;
			const int negPaddingIdx = MIN_PADDING - 1;
			const int posPaddingIdx = MIN_PADDING + coarseCells;
			// Loop also covers the -side padding row (minCell - 1) that Pass 1
			// now fills, so its boundary quads can collapse against a coarser
			// neighbour. When extendPositive[axis] is set we additionally
			// reach into the +side padding row so its vertices on a *different*
			// stitched face are snapped too (avoids T-junctions in axis corners
			// where this chunk both extends +side and stitches on another face).
			for (int z = minCell - 1; z < zMaxCell; ++z)
			{
				for (int y = minCell - 1; y < yMaxCell; ++y)
				{
					for (int x = minCell - 1; x < xMaxCell; ++x)
					{
						const int idx = cellVertexIndex[getIndex(x, y, z, voxelSize)];
						if (idx < 0) continue;

						const bool onNegX = (x <= boundaryMin) && stitchConfig->stitchFace[0];
						const bool onPosX = (x == boundaryMax) && stitchConfig->stitchFace[1];
						const bool onNegY = (y <= boundaryMin) && stitchConfig->stitchFace[2];
						const bool onPosY = (y == boundaryMax) && stitchConfig->stitchFace[3];
						const bool onNegZ = (z <= boundaryMin) && stitchConfig->stitchFace[4];
						const bool onPosZ = (z == boundaryMax) && stitchConfig->stitchFace[5];
						if (!(onNegX || onPosX || onNegY || onPosY || onNegZ || onPosZ)) continue;

						// Default per-axis: in-plane axes use the half-resolution
						// mapping to pick the coarse cell whose volume covers this
						// fine cell. Boundary axes are overridden below to reach
						// into the padding row that belongs to the coarser
						// neighbour chunk.
						//
						// Use floor division (not C++ truncation toward zero) so
						// that the -side padding row (fine index minCell - 1, i.e.
						// difference -1) maps to coarse cell MIN_PADDING - 1
						// instead of MIN_PADDING. With truncation, the corner
						// padding vertex on a stitched face would snap to the
						// +side neighbour coarse vertex, folding the very first
						// boundary triangle toward larger indices and leaving a
						// missing-corner artifact.
						auto floorDiv2 = [](int v) {
							return (v >= 0) ? (v / 2) : -((1 - v) / 2);
						};
						int cx = MIN_PADDING + floorDiv2(x - MIN_PADDING);
						int cy = MIN_PADDING + floorDiv2(y - MIN_PADDING);
						int cz = MIN_PADDING + floorDiv2(z - MIN_PADDING);
						if (onNegX) cx = negPaddingIdx;
						else if (onPosX) cx = posPaddingIdx;
						if (onNegY) cy = negPaddingIdx;
						else if (onPosY) cy = posPaddingIdx;
						if (onNegZ) cz = negPaddingIdx;
						else if (onPosZ) cz = posPaddingIdx;

						vec3 coarsePos;
						if (computeDualVertexPosition(basePoint,
								stitchConfig->coarseVoxelSize, stitchConfig->coarseData,
								stitchConfig->coarseLodLevel, cx, cy, cz, coarsePos))
						{
							mesh->m_vertices[idx].m_pos = coarsePos;
							mesh->m_vertices[idx].m_texCoord = coarsePos.xz();
						}
						// Otherwise the neighbour-side coarse cell carries no surface,
						// meaning the coarser neighbour chunk will not produce geometry
						// there either; leaving the fine vertex untouched is correct
						// (no opposing vertex exists to align with).
					}
				}
			}
		}
	}

	mesh->caclNormals();
}

} // namespace tzw
