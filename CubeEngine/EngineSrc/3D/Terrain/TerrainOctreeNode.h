#pragma once

#include "TerrainOctreeTypes.h"

#include <array>
#include <memory>

namespace tzw {

enum class TerrainMeshState
{
	Empty,
	Requested,
	Ready,
	Failed,
};

class TerrainOctreeNode
{
public:
	TerrainOctreeNode(const TerrainNodeKey& key, const TerrainRegion& region, TerrainOctreeNode* parent = nullptr);

	const TerrainNodeKey& key() const;
	const TerrainRegion& region() const;
	TerrainOctreeNode* parent() const;
	TerrainOctreeNode* child(int childIndex) const;
	TerrainOctreeNode* ensureChild(int childIndex, const TerrainOctreeConfig& config);

	bool subdivide(const TerrainOctreeConfig& config);
	void collapse();
	bool hasChildren() const;
	bool isLeaf() const;
	bool canSubdivide(const TerrainOctreeConfig& config) const;

	bool isDirty() const;
	void setDirty(bool isDirty);
	TerrainMeshState meshState() const;
	void setMeshState(TerrainMeshState state);
	bool isMeshRequested() const;
	void setMeshRequested(bool isRequested);
	bool canRender() const;
	void setCanRender(bool canRender);

private:
	TerrainNodeKey m_key;
	TerrainRegion m_region;
	TerrainOctreeNode* m_parent = nullptr;
	std::array<std::unique_ptr<TerrainOctreeNode>, 8> m_children;
	bool m_isDirty = true;
	bool m_isMeshRequested = false;
	bool m_canRender = true;
	TerrainMeshState m_meshState = TerrainMeshState::Empty;
};

} // namespace tzw
