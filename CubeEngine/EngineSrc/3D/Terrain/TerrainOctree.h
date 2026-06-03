#pragma once

#include "TerrainLodSelector.h"
#include "TerrainRenderSet.h"

#include <memory>

namespace tzw {

class TerrainOctree
{
public:
	TerrainOctree();
	explicit TerrainOctree(const TerrainOctreeConfig& config);

	void configure(const TerrainOctreeConfig& config);
	bool isValid() const;
	void update(const TerrainLodContext& context);
	void update(const vec3& viewerPosition);
	void clear();

	TerrainOctreeNode* root() const;
	TerrainOctreeNode* findNode(const TerrainNodeKey& key) const;
	TerrainOctreeNode* ensureNode(const TerrainNodeKey& key);
	bool markDirty(const TerrainNodeKey& key);

	const TerrainOctreeConfig& config() const;
	const TerrainRenderSet& renderSet() const;
	TerrainRenderSet& renderSet();
	TerrainLodSelector& lodSelector();
	const TerrainLodSelector& lodSelector() const;

private:
	void traverse(TerrainOctreeNode* node, const TerrainLodContext& context);

	TerrainOctreeConfig m_config;
	std::unique_ptr<TerrainOctreeNode> m_root;
	TerrainLodSelector m_lodSelector;
	TerrainRenderSet m_renderSet;
};

} // namespace tzw
