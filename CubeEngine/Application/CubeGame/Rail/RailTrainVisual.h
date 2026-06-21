#pragma once

#include "RailTrain.h"

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

private:
	struct TrainVisual
	{
		RailTrainId trainId = InvalidRailTrainId;
		std::vector<CubePrimitive*> cars;
	};

	void ensureVisualRoot(Node* sceneRoot);
	TrainVisual& ensureTrainVisual(RailTrainId trainId);
	void ensureCarCount(TrainVisual& visual, int carCount);
	void hideUnusedTrains(const RailTrainManager& trainManager);

	Node* m_visualRoot = nullptr;
	std::vector<TrainVisual> m_trainVisuals;
};

} // namespace tzw
