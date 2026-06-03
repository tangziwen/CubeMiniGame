#include "TerrainOctreeNode.h"

namespace tzw {

TerrainOctreeNode::TerrainOctreeNode(const TerrainNodeKey& key, const TerrainRegion& region, TerrainOctreeNode* parent)
	: m_key(key), m_region(region), m_parent(parent)
{
}

const TerrainNodeKey& TerrainOctreeNode::key() const
{
	return m_key;
}

const TerrainRegion& TerrainOctreeNode::region() const
{
	return m_region;
}

TerrainOctreeNode* TerrainOctreeNode::parent() const
{
	return m_parent;
}

TerrainOctreeNode* TerrainOctreeNode::child(int childIndex) const
{
	if (childIndex < 0 || childIndex >= static_cast<int>(m_children.size()))
	{
		return nullptr;
	}
	return m_children[childIndex].get();
}

TerrainOctreeNode* TerrainOctreeNode::ensureChild(int childIndex, const TerrainOctreeConfig& config)
{
	if (childIndex < 0 || childIndex >= static_cast<int>(m_children.size()) || !canSubdivide(config))
	{
		return nullptr;
	}

	if (!m_children[childIndex])
	{
		const TerrainNodeKey childKey = m_key.child(childIndex);
		m_children[childIndex] = std::make_unique<TerrainOctreeNode>(childKey, config.regionForKey(childKey), this);
	}
	return m_children[childIndex].get();
}

bool TerrainOctreeNode::subdivide(const TerrainOctreeConfig& config)
{
	if (!canSubdivide(config))
	{
		return false;
	}

	for (int i = 0; i < static_cast<int>(m_children.size()); ++i)
	{
		ensureChild(i, config);
	}
	return true;
}

void TerrainOctreeNode::collapse()
{
	for (auto& child : m_children)
	{
		child.reset();
	}
}

bool TerrainOctreeNode::hasChildren() const
{
	for (const auto& child : m_children)
	{
		if (child)
		{
			return true;
		}
	}
	return false;
}

bool TerrainOctreeNode::isLeaf() const
{
	return !hasChildren();
}

bool TerrainOctreeNode::canSubdivide(const TerrainOctreeConfig& config) const
{
	return config.isValid() && m_key.level < config.maxDepth && m_region.cellSize > config.leafCellSize;
}

bool TerrainOctreeNode::isDirty() const
{
	return m_isDirty;
}

void TerrainOctreeNode::setDirty(bool isDirty)
{
	m_isDirty = isDirty;
}

TerrainMeshState TerrainOctreeNode::meshState() const
{
	return m_meshState;
}

void TerrainOctreeNode::setMeshState(TerrainMeshState state)
{
	m_meshState = state;
}

bool TerrainOctreeNode::isMeshRequested() const
{
	return m_isMeshRequested;
}

void TerrainOctreeNode::setMeshRequested(bool isRequested)
{
	m_isMeshRequested = isRequested;
	if (isRequested && m_meshState == TerrainMeshState::Empty)
	{
		m_meshState = TerrainMeshState::Requested;
	}
}

bool TerrainOctreeNode::canRender() const
{
	return m_canRender;
}

void TerrainOctreeNode::setCanRender(bool canRender)
{
	m_canRender = canRender;
}

} // namespace tzw
