#include "TerrainRenderSet.h"

namespace tzw {

void TerrainRenderSet::clear()
{
	m_nodes.clear();
	m_keys.clear();
	m_nodeByKey.clear();
}

bool TerrainRenderSet::addNode(TerrainOctreeNode* node)
{
	if (!node || conflictsWith(node->key()))
	{
		return false;
	}

	m_nodes.push_back(node);
	m_keys.insert(node->key());
	m_nodeByKey[node->key()] = node;
	return true;
}

bool TerrainRenderSet::removeNode(const TerrainNodeKey& key)
{
	if (m_keys.find(key) == m_keys.end())
	{
		return false;
	}

	m_keys.erase(key);
	m_nodeByKey.erase(key);
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		if (*it && (*it)->key() == key)
		{
			m_nodes.erase(it);
			return true;
		}
	}
	return false;
}

TerrainOctreeNode* TerrainRenderSet::findNode(const TerrainNodeKey& key) const
{
	auto it = m_nodeByKey.find(key);
	return it != m_nodeByKey.end() ? it->second : nullptr;
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
