#include "TerrainOctree.h"
#include "Transvoxel.h"

namespace tzw {

TerrainOctree::TerrainOctree()
{
	configure(TerrainOctreeConfig());
}

TerrainOctree::TerrainOctree(const TerrainOctreeConfig& config)
{
	configure(config);
}

void TerrainOctree::configure(const TerrainOctreeConfig& config)
{
	m_config = config;
	m_renderSet.clear();
	if (m_config.isValid())
	{
		m_root = std::make_unique<TerrainOctreeNode>(TerrainNodeKey(), m_config.rootRegion());
	}
	else
	{
		m_root.reset();
	}
}

bool TerrainOctree::isValid() const
{
	return m_root != nullptr && m_config.isValid();
}

void TerrainOctree::update(const TerrainLodContext& context)
{
	m_renderSet.clear();
	if (!isValid())
	{
		return;
	}
	traverse(m_root.get(), context);
}

void TerrainOctree::update(const vec3& viewerPosition)
{
	TerrainLodContext context;
	context.viewerPosition = viewerPosition;
	update(context);
}

void TerrainOctree::clear()
{
	m_renderSet.clear();
	m_root.reset();
}

TerrainOctreeNode* TerrainOctree::root() const
{
	return m_root.get();
}

TerrainOctreeNode* TerrainOctree::findNode(const TerrainNodeKey& key) const
{
	if (!m_root || key.level < 0 || key.level > m_config.maxDepth)
	{
		return nullptr;
	}
	const int maxCoord = 1 << key.level;
	if (key.x < 0 || key.y < 0 || key.z < 0 || key.x >= maxCoord || key.y >= maxCoord || key.z >= maxCoord)
	{
		return nullptr;
	}

	TerrainOctreeNode* current = m_root.get();
	for (int level = key.level; level > 0 && current; --level)
	{
		const int shift = level - 1;
		const int childIndex = ((key.x >> shift) & 1)
			| (((key.y >> shift) & 1) << 1)
			| (((key.z >> shift) & 1) << 2);
		current = current->child(childIndex);
	}
	return current;
}

TerrainOctreeNode* TerrainOctree::ensureNode(const TerrainNodeKey& key)
{
	if (!m_root || key.level < 0 || key.level > m_config.maxDepth)
	{
		return nullptr;
	}
	const int maxCoord = 1 << key.level;
	if (key.x < 0 || key.y < 0 || key.z < 0 || key.x >= maxCoord || key.y >= maxCoord || key.z >= maxCoord)
	{
		return nullptr;
	}

	TerrainOctreeNode* current = m_root.get();
	for (int level = key.level; level > 0 && current; --level)
	{
		const int shift = level - 1;
		const int childIndex = ((key.x >> shift) & 1)
			| (((key.y >> shift) & 1) << 1)
			| (((key.z >> shift) & 1) << 2);
		current = current->ensureChild(childIndex, m_config);
	}
	return current;
}

bool TerrainOctree::markDirty(const TerrainNodeKey& key)
{
	TerrainOctreeNode* node = ensureNode(key);
	if (!node)
	{
		return false;
	}
	node->setDirty(true);
	return true;
}

int TerrainOctree::markDirtyInBounds(const TerrainEditBounds& bounds)
{
	if (!isValid())
	{
		return 0;
	}
	int count = 0;
	markDirtyInBoundsRecursive(m_root.get(), bounds, count);
	return count;
}

void TerrainOctree::markDirtyInBoundsRecursive(TerrainOctreeNode* node, const TerrainEditBounds& bounds, int& count)
{
	if (!node)
	{
		return;
	}

	const int stride = node->region().sampleStride(m_config.meshCellCount);
	const int s = std::max(stride, 1);

	const TerrainInt3 cellMax = node->region().cellMaxExclusive();
	const TerrainInt3 paddedMin(
		node->region().voxelMin.x - MIN_PADDING * s,
		node->region().voxelMin.y - MIN_PADDING * s,
		node->region().voxelMin.z - MIN_PADDING * s);
	const TerrainInt3 paddedMax(
		cellMax.x + MAX_PADDING * s,
		cellMax.y + MAX_PADDING * s,
		cellMax.z + MAX_PADDING * s);

	if (bounds.voxelMaxExclusive.x <= paddedMin.x || bounds.voxelMin.x >= paddedMax.x
		|| bounds.voxelMaxExclusive.y <= paddedMin.y || bounds.voxelMin.y >= paddedMax.y
		|| bounds.voxelMaxExclusive.z <= paddedMin.z || bounds.voxelMin.z >= paddedMax.z)
	{
		return;
	}

	node->setDirty(true);
	++count;

	for (int i = 0; i < 8; ++i)
	{
		markDirtyInBoundsRecursive(node->child(i), bounds, count);
	}
}

const TerrainOctreeConfig& TerrainOctree::config() const
{
	return m_config;
}

const TerrainRenderSet& TerrainOctree::renderSet() const
{
	return m_renderSet;
}

TerrainRenderSet& TerrainOctree::renderSet()
{
	return m_renderSet;
}

TerrainLodSelector& TerrainOctree::lodSelector()
{
	return m_lodSelector;
}

const TerrainLodSelector& TerrainOctree::lodSelector() const
{
	return m_lodSelector;
}

void TerrainOctree::traverse(TerrainOctreeNode* node, const TerrainLodContext& context)
{
	if (!node)
	{
		return;
	}

	const TerrainLodDecision decision = m_lodSelector.select(*node, m_config, context);
	if (decision == TerrainLodDecision::DelayLoad)
	{
		return;
	}

	if (decision == TerrainLodDecision::Subdivide && node->subdivide(m_config))
	{
		for (int i = 0; i < 8; ++i)
		{
			traverse(node->child(i), context);
		}
		return;
	}

	m_renderSet.addNode(node);
}

} // namespace tzw
