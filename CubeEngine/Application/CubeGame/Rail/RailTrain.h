#pragma once

#include "RailLine.h"

#include <functional>
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

struct RailTrainEvent
{
	RailTrainId trainId = InvalidRailTrainId;
	RailLineId lineId = InvalidRailLineId;
	RailStationId stationId = InvalidRailStationId;
	RailPlatformId platformId = InvalidRailPlatformId;
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
	bool isManuallyStopped = false;
	bool isDwelling = false;
	float dwellRemainingSeconds = 0.0f;
	RailStationId dwellingStationId = InvalidRailStationId;
	RailPlatformId dwellingPlatformId = InvalidRailPlatformId;
	RailStationId lastStoppedStationId = InvalidRailStationId;
	RailPlatformId lastStoppedPlatformId = InvalidRailPlatformId;
	float lastStopDistance = -1.0f;
	RailTrainPose pose;
	std::vector<RailTrainPose> carPoses;
};

class RailTrainManager
{
public:
	RailTrainId createTrain(int carriageCount);
	bool unserializeTrain(const RailTrain& train);
	bool deleteTrain(RailTrainId trainId);
	bool assignLine(RailTrainId trainId, RailLineId lineId, const RailLineManager& lineManager);
	void clearLineAssignment(RailLineId lineId);
	void update(float deltaSeconds, const RailLineManager& lineManager, const RailStationManager& stationManager);
	void updateWorldPoses(const RailNetwork& network, const RailLineManager& lineManager);
	void refreshAfterLineRebuild(const RailLineManager& lineManager);
	void clear();

	RailTrain* train(RailTrainId trainId);
	const RailTrain* train(RailTrainId trainId) const;
	const std::vector<RailTrain>& trains() const;
	bool setTrainRunning(RailTrainId trainId, bool isRunning);
	bool toggleTrainRunning(RailTrainId trainId);
	void addTrainArrivedListener(std::function<void(const RailTrainEvent&)> listener);
	void addTrainDepartedListener(std::function<void(const RailTrainEvent&)> listener);

private:
	struct StationStopCandidate
	{
		bool valid = false;
		RailStationId stationId = InvalidRailStationId;
		RailPlatformId platformId = InvalidRailPlatformId;
		float distanceOnLine = 0.0f;
	};

	void clampTrainToLine(RailTrain& train, const RailLine& line);
	StationStopCandidate findNextStationStop(const RailTrain& train, const RailLine& line,
		const RailStationManager& stationManager, float oldDistance, float newDistance) const;
	void startDwelling(RailTrain& train, const StationStopCandidate& stop);
	void finishDwelling(RailTrain& train);
	void notifyArrived(const RailTrain& train);
	void notifyDeparted(const RailTrain& train);

	std::vector<RailTrain> m_trains;
	std::vector<std::function<void(const RailTrainEvent&)>> m_trainArrivedListeners;
	std::vector<std::function<void(const RailTrainEvent&)>> m_trainDepartedListeners;
	RailTrainId m_nextTrainId = 1;
};

} // namespace tzw
