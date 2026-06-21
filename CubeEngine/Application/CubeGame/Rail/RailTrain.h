#pragma once

#include "RailLine.h"

#include <string>
#include <vector>

namespace tzw {

using RailTrainId = int;

constexpr RailTrainId InvalidRailTrainId = -1;

enum class RailTrainPlacementMode
{
	Unplaced,
	OnLine,
	Detached,
};

struct RailTrainPose
{
	bool valid = false;
	vec3 position;
	vec3 tangent = vec3(1.0f, 0.0f, 0.0f);
};

class RailTrain
{
public:
	RailTrainId id = InvalidRailTrainId;
	std::string name;
	RailLineId lineId = InvalidRailLineId;
	RailTrainPlacementMode placementMode = RailTrainPlacementMode::Unplaced;
	int carriageCount = 0;
	float speed = 2.5f;
	float headDistance = 0.0f;
	int direction = 1;
	bool active = false;
	RailTrainPose pose;
	std::vector<RailTrainPose> carPoses;
};

class RailTrainManager
{
public:
	RailTrainId createTrain(int carriageCount);
	bool deleteTrain(RailTrainId trainId);
	bool assignLine(RailTrainId trainId, RailLineId lineId, const RailLineManager& lineManager);
	void clearLineAssignment(RailLineId lineId);
	void update(float deltaSeconds, const RailLineManager& lineManager);
	void updateWorldPoses(const RailNetwork& network, const RailLineManager& lineManager);
	void refreshAfterLineRebuild(const RailLineManager& lineManager);
	void clear();

	RailTrain* train(RailTrainId trainId);
	const RailTrain* train(RailTrainId trainId) const;
	const std::vector<RailTrain>& trains() const;

private:
	void clampTrainToLine(RailTrain& train, const RailLine& line);

	std::vector<RailTrain> m_trains;
	RailTrainId m_nextTrainId = 1;
};

} // namespace tzw
