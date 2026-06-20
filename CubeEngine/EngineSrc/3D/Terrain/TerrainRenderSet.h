#pragma once

#include "TerrainOctreeNode.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace tzw {

class TerrainRenderSet
{
public:
	using NodeList = std::vector<TerrainOctreeNode*>;

	void clear();
	bool addNode(TerrainOctreeNode* node);
	bool removeNode(const TerrainNodeKey& key);
	TerrainOctreeNode* findNode(const TerrainNodeKey& key) const;
	bool contains(const TerrainNodeKey& key) const;
	bool conflictsWith(const TerrainNodeKey& key) const;
	const NodeList& nodes() const;
	size_t size() const;
	bool empty() const;

private:
	NodeList m_nodes;
	std::unordered_set<TerrainNodeKey> m_keys;
	std::unordered_map<TerrainNodeKey, TerrainOctreeNode*> m_nodeByKey;
};

} // namespace tzw
