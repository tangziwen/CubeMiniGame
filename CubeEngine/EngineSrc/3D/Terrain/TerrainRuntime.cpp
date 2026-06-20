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
#include "TerrainNeighborResolver.h"
#include "../../Base/Node.h"
#include "../../Technique/MaterialPool.h"
#include "GameMap.h"

#include <algorithm>
#include <vector>

namespace tzw {

namespace
{

void faceTangentialAxes(TerrainNeighborFace face, int& tangent0, int& tangent1)
{
	const int axis = static_cast<int>(face) / 2;
	tangent0 = (axis + 1) % 3;
	tangent1 = (axis + 2) % 3;
}

TerrainNeighborFace faceForAxisSide(int axis, int side)
{
	return static_cast<TerrainNeighborFace>(axis * 2 + (side > 0 ? 1 : 0));
}

uint8_t stitchPreserveFineBorderBit(int tangentIndex, int side)
{
	return static_cast<uint8_t>(1u << (tangentIndex * 2 + (side > 0 ? 1 : 0)));
}

uint8_t stitchPreserveFineBorderMask(const TerrainOctree& octree,
	const TerrainRenderSet& renderSet,
	const std::array<TerrainNeighborRelation, 6>& neighbors,
	TerrainNeighborResolver& resolver, TerrainNeighborFace stitchFace)
{
	uint8_t mask = 0;
	int tangentAxes[2] = { 0, 0 };
	faceTangentialAxes(stitchFace, tangentAxes[0], tangentAxes[1]);
	for (int tangentIndex = 0; tangentIndex < 2; ++tangentIndex)
	{
		const int sides[2] = { -1, 1 };
		for (int side : sides)
		{
			const TerrainNeighborFace tangentFace = faceForAxisSide(
				tangentAxes[tangentIndex], side);
			const TerrainNeighborRelation& tangentRelation =
				neighbors[static_cast<int>(tangentFace)];
			if (!tangentRelation.isSameLevel || !tangentRelation.node)
			{
				continue;
			}

			const TerrainNeighborRelation tangentStitchRelation =
				resolver.findNeighbor(octree, renderSet, *tangentRelation.node, stitchFace);
			if (!tangentStitchRelation.isCoarser || tangentStitchRelation.levelDelta != -1)
			{
				mask |= stitchPreserveFineBorderBit(tangentIndex, side);
			}
		}
	}
	return mask;
}

TerrainMeshSeamSet makeTerrainMeshSeams(const TerrainOctree& octree,
	const TerrainRenderSet& renderSet, const TerrainOctreeNode& node)
{
	TerrainMeshSeamSet seams;
	TerrainNeighborResolver resolver;
	const auto neighbors = resolver.findNeighbors(octree, renderSet, node);
	for (size_t i = 0; i < neighbors.size(); ++i)
	{
		const auto& relation = neighbors[i];
		TerrainMeshSeamFace& face = seams.faces[i];
		face.levelDelta = relation.levelDelta;

		if (relation.isBoundary || !relation.exists)
		{
			face.mode = TerrainMeshSeamMode::Boundary;
		}
		else if (relation.isSameLevel)
		{
			face.mode = TerrainMeshSeamMode::SameLevel;
		}
		else if (relation.isCoarser && relation.levelDelta == -1)
		{
			face.mode = TerrainMeshSeamMode::StitchToCoarser;
			face.stitchPreserveFineBorderMask = stitchPreserveFineBorderMask(
				octree, renderSet, neighbors, resolver,
				static_cast<TerrainNeighborFace>(i));
		}
		else if (relation.isFiner && relation.levelDelta == 1)
		{
			face.mode = TerrainMeshSeamMode::SuppressForFiner;
			face.finerCoverageMask = relation.finerCoverageMask;
		}
		else
		{
			face.mode = TerrainMeshSeamMode::Invalid;
		}
	}
	return seams;
}

struct TerrainMeshBuildCandidate
{
	TerrainOctreeNode* node = nullptr;
	TerrainMeshSeamSet seams;
	float distanceSq = 0.0f;
	int sampleLodPower = 0;
};

} // namespace

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
	m_mesher->setDebugLodVertexColorEnabled(m_debugLodVertexColorEnabled);
	m_mesher->setDebugLodCheckerboardEnabled(m_debugLodCheckerboardEnabled);
	m_drawableSet = std::make_unique<TerrainDrawableSet>();
	m_editSystem = std::make_unique<TerrainEditSystem>();

	m_editSystem->bind(GameMap::shared(), m_octree.get(), m_meshCache.get());

	auto material = MaterialPool::shared()->getMatFromTemplate("VoxelTerrain");
	m_drawableSet->setMaterial(material);

	m_runtimeSeconds = 0.0f;
}

void TerrainRuntime::update(const vec3& viewerPosition, Node* sceneRoot, float deltaSeconds)
{
	if (!m_octree || !m_octree->isValid() || !sceneRoot)
	{
		return;
	}

	m_runtimeSeconds += std::max(deltaSeconds, 0.0f);

	// 1. Octree traversal based on viewer position
	if (m_lodViewerPositionFrozen && !m_hasFrozenLodViewerPosition)
	{
		m_frozenLodViewerPosition = viewerPosition;
		m_hasFrozenLodViewerPosition = true;
	}
	const vec3& lodViewerPosition = m_lodViewerPositionFrozen
		? m_frozenLodViewerPosition
		: viewerPosition;
	TerrainLodContext lodContext;
	lodContext.viewerPosition = lodViewerPosition;
	lodContext.maxViewDistance = m_viewDistance;
	m_octree->update(lodContext);

	const TerrainRenderSet& renderSet = m_octree->renderSet();

	// 2. Touch current render set in mesh cache
	m_meshCache->touchRenderSet(renderSet, m_runtimeSeconds);

	// 3. Collect and sort mesh generation candidates before spending the per-frame build budget.
	std::vector<TerrainMeshBuildCandidate> buildCandidates;
	for (TerrainOctreeNode* node : renderSet.nodes())
	{
		if (!node)
		{
			continue;
		}

		const TerrainNodeKey& key = node->key();
		const TerrainMeshCacheEntry* entry = m_meshCache->find(key);

		// Compute current seam set before deciding if a rebuild is needed.
		TerrainMeshSeamSet seams = makeTerrainMeshSeams(*m_octree, renderSet, *node);

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
		else if ((entry->state == TerrainMeshState::Ready || entry->state == TerrainMeshState::Failed)
			&& entry->request.seamSignature != seams.signature())
		{
			needRequest = true;
		}

		if (needRequest)
		{
			const AABB worldBounds = node->region().worldAABB(
				m_octree->config().mapOffset, m_octree->config().blockSize);
			TerrainMeshBuildCandidate candidate;
			candidate.node = node;
			candidate.seams = seams;
			candidate.distanceSq = worldBounds.distanceSquaredToPoint(viewerPosition);
			candidate.sampleLodPower = node->region().sampleLodPower(m_octree->config().meshCellCount);
			buildCandidates.push_back(candidate);
		}
	}

	std::sort(buildCandidates.begin(), buildCandidates.end(),
		[](const TerrainMeshBuildCandidate& left, const TerrainMeshBuildCandidate& right)
	{
		if (left.distanceSq != right.distanceSq)
		{
			return left.distanceSq < right.distanceSq;
		}
		if (left.sampleLodPower != right.sampleLodPower)
		{
			return left.sampleLodPower < right.sampleLodPower;
		}
		if (left.node && right.node && left.node->key().level != right.node->key().level)
		{
			return left.node->key().level > right.node->key().level;
		}
		if (left.node && right.node)
		{
			return left.node->key() < right.node->key();
		}
		return left.node != nullptr;
	});

	const int maxSyncMeshBuildsPerFrame = 2;
	int syncMeshBuildsThisFrame = 0;
	for (const TerrainMeshBuildCandidate& candidate : buildCandidates)
	{
		if (!candidate.node || syncMeshBuildsThisFrame >= maxSyncMeshBuildsPerFrame)
		{
			break;
		}

		TerrainMeshRequest request = m_meshCache->makeRequest(
			*candidate.node, m_octree->config(), candidate.seams);
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

		candidate.node->setDirty(false);
		++syncMeshBuildsThisFrame;
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
	m_meshCache->evictUnused(renderSet, m_octree->config(), viewerPosition, m_runtimeSeconds,
		m_viewDistance + m_cacheUnloadMargin, m_cacheKeepAliveSeconds);
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
	m_runtimeSeconds = 0.0f;
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

void TerrainRuntime::setDebugLodVertexColorEnabled(bool enabled)
{
	if (m_debugLodVertexColorEnabled == enabled)
	{
		return;
	}

	m_debugLodVertexColorEnabled = enabled;
	if (m_mesher)
	{
		m_mesher->setDebugLodVertexColorEnabled(enabled);
	}
	if (m_drawableSet)
	{
		m_drawableSet->clear();
	}
	if (m_meshCache)
	{
		m_meshCache->clear();
	}
}

bool TerrainRuntime::isDebugLodVertexColorEnabled() const
{
	return m_debugLodVertexColorEnabled;
}

void TerrainRuntime::setDebugLodCheckerboardEnabled(bool enabled)
{
	if (m_debugLodCheckerboardEnabled == enabled)
	{
		return;
	}

	m_debugLodCheckerboardEnabled = enabled;
	if (m_mesher)
	{
		m_mesher->setDebugLodCheckerboardEnabled(enabled);
	}
	if (m_drawableSet)
	{
		m_drawableSet->clear();
	}
	if (m_meshCache)
	{
		m_meshCache->clear();
	}
}

bool TerrainRuntime::isDebugLodCheckerboardEnabled() const
{
	return m_debugLodCheckerboardEnabled;
}

void TerrainRuntime::setDebugWireframeEnabled(bool enabled)
{
	if (m_debugWireframeEnabled == enabled)
	{
		return;
	}

	m_debugWireframeEnabled = enabled;
	if (m_drawableSet)
	{
		m_drawableSet->setDebugWireframeEnabled(enabled);
	}
}

bool TerrainRuntime::isDebugWireframeEnabled() const
{
	return m_debugWireframeEnabled;
}

void TerrainRuntime::setLodViewerPositionFreezeEnabled(bool enabled)
{
	if (enabled)
	{
		m_lodViewerPositionFrozen = true;
		m_hasFrozenLodViewerPosition = false;
		return;
	}
	unfreezeLodViewerPosition();
}

void TerrainRuntime::freezeLodViewerPosition(const vec3& viewerPosition)
{
	m_frozenLodViewerPosition = viewerPosition;
	m_lodViewerPositionFrozen = true;
	m_hasFrozenLodViewerPosition = true;
}

void TerrainRuntime::unfreezeLodViewerPosition()
{
	m_lodViewerPositionFrozen = false;
	m_hasFrozenLodViewerPosition = false;
}

bool TerrainRuntime::isLodViewerPositionFrozen() const
{
	return m_lodViewerPositionFrozen;
}

const vec3& TerrainRuntime::frozenLodViewerPosition() const
{
	return m_frozenLodViewerPosition;
}

void TerrainRuntime::setViewDistance(float distance)
{
	m_viewDistance = distance;
}

float TerrainRuntime::viewDistance() const
{
	return m_viewDistance;
}

void TerrainRuntime::setCacheUnloadMargin(float margin)
{
	m_cacheUnloadMargin = std::max(margin, 0.0f);
}

float TerrainRuntime::cacheUnloadMargin() const
{
	return m_cacheUnloadMargin;
}

void TerrainRuntime::setCacheKeepAliveSeconds(float seconds)
{
	m_cacheKeepAliveSeconds = std::max(seconds, 0.0f);
}

float TerrainRuntime::cacheKeepAliveSeconds() const
{
	return m_cacheKeepAliveSeconds;
}

} // namespace tzw
