#include "TerrainRuntime.h"

#include "TerrainOctree.h"
#include "TerrainMeshCache.h"
#include "TerrainSampler.h"
#include "TerrainMesher.h"
#include "TerrainDrawableSet.h"
#include "TerrainEditSystem.h"
#include "TerrainOctreeTypes.h"
#include "TerrainRenderSet.h"
#include "TerrainOctreeNode.h"
#include "../../Base/Node.h"
#include "../../Technique/MaterialPool.h"
#include "GameMap.h"

namespace tzw {

TerrainRuntime::TerrainRuntime()
{
}

TerrainRuntime::~TerrainRuntime()
{
}

void TerrainRuntime::init(const TerrainOctreeConfig& config)
{
	if (!config.isValid())
	{
		return;
	}

	m_octree = std::make_unique<TerrainOctree>(config);
	m_meshCache = std::make_unique<TerrainMeshCache>();
	m_sampler = std::make_unique<TerrainSampler>(config.domainSize);
	m_mesher = std::make_unique<TerrainMesher>();
	m_drawableSet = std::make_unique<TerrainDrawableSet>();
	m_editSystem = std::make_unique<TerrainEditSystem>();

	m_editSystem->bind(GameMap::shared(), m_octree.get(), m_meshCache.get());

	auto material = MaterialPool::shared()->getMatFromTemplate("VoxelTerrain");
	m_drawableSet->setMaterial(material);

	m_frameIndex = 0;
}

void TerrainRuntime::update(const vec3& viewerPosition, Node* sceneRoot)
{
	if (!m_octree || !m_octree->isValid() || !sceneRoot)
	{
		return;
	}

	// 1. Octree traversal based on viewer position
	m_octree->update(viewerPosition);

	const TerrainRenderSet& renderSet = m_octree->renderSet();

	// 2. Touch current render set in mesh cache
	m_meshCache->touchRenderSet(renderSet, m_frameIndex);

	// 3. Generate mesh for nodes that lack a ready mesh
	const int maxSyncMeshBuildsPerFrame = 2;
	int syncMeshBuildsThisFrame = 0;
	for (TerrainOctreeNode* node : renderSet.nodes())
	{
		if (!node)
		{
			continue;
		}

		const TerrainNodeKey& key = node->key();
		const TerrainMeshCacheEntry* entry = m_meshCache->find(key);

		bool needRequest = false;
		if (!entry)
		{
			needRequest = true;
		}
		else if (entry->state == TerrainMeshState::Empty)
		{
			needRequest = true;
		}
		else if (node->isDirty() && entry->state != TerrainMeshState::Requested)
		{
			needRequest = true;
		}

		if (needRequest)
		{
			if (syncMeshBuildsThisFrame >= maxSyncMeshBuildsPerFrame)
			{
				continue;
			}
			TerrainMeshRequest request = m_meshCache->makeRequest(*node, m_octree->config());
			m_meshCache->markRequested(request);

			// First version: synchronous generation
			TerrainSampleBuffer sampleBuffer;
			if (m_sampler->sample(request, sampleBuffer))
			{
				TerrainMeshBuildResult result = m_mesher->build(request, sampleBuffer);
				if (result.state == TerrainMeshState::Ready)
				{
					m_meshCache->storeReady(request, std::move(result.mesh));
				}
				else
				{
					m_meshCache->markFailed(request);
				}
			}
			else
			{
				m_meshCache->markFailed(request);
			}

			node->setDirty(false);
			++syncMeshBuildsThisFrame;
		}
	}

	// 4. Finish ready meshes on the render thread before drawables can submit commands.
	m_meshCache->finishReadyMeshesForRender(renderSet);

	// 5. Sync drawable set with render set and mesh cache
	m_drawableSet->sync(renderSet, *m_meshCache);

	// 6. Attach new drawables to scene root
	for (auto& pair : m_drawableSet->drawables())
	{
		if (pair.second && !pair.second->getParent())
		{
			sceneRoot->addChild(pair.second.get());
		}
	}

	// 7. Evict unused cache entries
	m_meshCache->evictUnused(m_frameIndex, 60);

	++m_frameIndex;
}

void TerrainRuntime::clear()
{
	if (m_drawableSet)
	{
		m_drawableSet->clear();
	}
	if (m_meshCache)
	{
		m_meshCache->clear();
	}
	m_octree.reset();
	m_frameIndex = 0;
}

TerrainOctree* TerrainRuntime::octree() const
{
	return m_octree.get();
}

TerrainMeshCache* TerrainRuntime::meshCache() const
{
	return m_meshCache.get();
}

TerrainSampler* TerrainRuntime::sampler() const
{
	return m_sampler.get();
}

TerrainMesher* TerrainRuntime::mesher() const
{
	return m_mesher.get();
}

TerrainDrawableSet* TerrainRuntime::drawableSet() const
{
	return m_drawableSet.get();
}

TerrainEditSystem* TerrainRuntime::editSystem() const
{
	return m_editSystem.get();
}

bool TerrainRuntime::isActive() const
{
	return m_octree != nullptr && m_octree->isValid();
}

} // namespace tzw
