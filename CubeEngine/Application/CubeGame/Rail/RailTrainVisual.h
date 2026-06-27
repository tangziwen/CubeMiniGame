#pragma once

#include "RailTrain.h"
#include "EngineSrc/Math/Quaternion.h"
#include "EngineSrc/Math/vec3.h"
#include "EngineSrc/Math/vec4.h"
#include "EngineSrc/Scene/ScenePickingSystem.h"

#include <vector>

namespace tzw {

class CubePrimitive;
class Node;

class RailTrainVisualSet
{
public:
	~RailTrainVisualSet();

	void sync(Node* sceneRoot, const RailNetwork& network, const RailLineManager& lineManager,
		const RailTrainManager& trainManager);
	void clear();
	void setTrainOutline(RailTrainId trainId, bool enabled, vec4 color);
	void clearOutlines();

private:
	struct TrainVisual
	{
		RailTrainId trainId = InvalidRailTrainId;
		std::vector<CubePrimitive*> cars;
		std::vector<PickHandle> carPickHandles;
		bool outlineEnabled = false;
		vec4 outlineColor = vec4(1.0f, 0.85f, 0.15f, 1.0f);
	};

	void ensureVisualRoot(Node* sceneRoot);
	TrainVisual& ensureTrainVisual(RailTrainId trainId);
	void ensureCarCount(TrainVisual& visual, int carCount);
	void applyOutline(TrainVisual& visual);
	void hideUnusedTrains(const RailTrainManager& trainManager);
	void syncCarPickTarget(TrainVisual& visual, int carIndex, const vec3& center, const Quaternion& rotation);
	void setCarPickTargetEnabled(TrainVisual& visual, int carIndex, bool enabled);
	void clearPickTargets(TrainVisual& visual);

	Node* m_visualRoot = nullptr;
	std::vector<TrainVisual> m_trainVisuals;
};

} // namespace tzw
