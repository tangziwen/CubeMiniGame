#include "TerrainRenderSet.h"

namespace tzw {

void TerrainRenderSet::clear()
{
	m_nodes.clear();
	m_keys.clear();
}

bool TerrainRenderSet::addNode(TerrainOctreeNode* node)
{
	if (!node || conflictsWith(node->key()))
	{
		return false;
	}

	m_nodes.push_back(node);
	m_keys.insert(node->key());
	return true;
}

TerrainOctreeNode* TerrainRenderSet::findNode(const TerrainNodeKey& key) const
{
	for (TerrainOctreeNode* node : m_nodes)
	{
		if (node && node->key() == key)
		{
			return node;
		}
	}
	return nullptr;
}

bool TerrainRenderSet::contains(const TerrainNodeKey& key) const
{
	return m_keys.find(key) != m_keys.end();
}

bool TerrainRenderSet::conflictsWith(const TerrainNodeKey& key) const
{
	for (const TerrainNodeKey& existing : m_keys)
	{
		if (existing == key || existing.isAncestorOf(key) || existing.isDescendantOf(key))
		{
			return true;
		}
	}
	return false;
}

const TerrainRenderSet::NodeList& TerrainRenderSet::nodes() const
{
	return m_nodes;
}

size_t TerrainRenderSet::size() const
{
	return m_nodes.size();
}

bool TerrainRenderSet::empty() const
{
	return m_nodes.empty();
}

} // namespace tzw
