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

}

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
				if (mask == 0 || mask == 0xff) continue;

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
				if (count == 0) continue;

				const vec3 center = sum / static_cast<float>(count);
				const vec3 finalPosition = basePoint
					+ (center - vec3(static_cast<float>(minCell)))
					* cellWorldSize;
				const int matX = std::clamp(static_cast<int>(center.x), 0, voxelSize - 1);
				const int matY = std::clamp(static_cast<int>(center.y), 0, voxelSize - 1);
				const int matZ = std::clamp(static_cast<int>(center.z), 0, voxelSize - 1);
				mesh->addVertex(makeVertexData(finalPosition, *getVoxel(srcData, voxelSize, matX, matY, matZ)));
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
					&& y != maxCell - 1)
				{
					const unsigned char valY = getValue(srcData, voxelSize, x, y + 1, z);
					if (selfIn != (valY < isoLevel))
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
					&& z != maxCell - 1)
				{
					const unsigned char valZ = getValue(srcData, voxelSize, x, y, z + 1);
					if (selfIn != (valZ < isoLevel))
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

	mesh->caclNormals();
}

} // namespace tzw
