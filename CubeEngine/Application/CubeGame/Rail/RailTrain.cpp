#include "RailTrain.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace tzw {

namespace
{
constexpr float TrainCarSpacing = 1.25f;
constexpr float StationDwellSeconds = 5.0f;
constexpr float StopTriggerEpsilon = 0.02f;
constexpr float StopCooldownDistance = 0.25f;
}

RailTrainId RailTrainManager::createTrain(int carriageCount)
{
	RailTrain train;
	train.id = m_nextTrainId++;
	train.name = "Train " + std::to_string(train.id);
	train.carriageCount = std::max(0, carriageCount);
	m_trains.push_back(train);
	return train.id;
}

bool RailTrainManager::unserializeTrain(const RailTrain& train)
{
	if (train.id == InvalidRailTrainId || this->train(train.id))
	{
		return false;
	}
	RailTrain unserialized = train;
	unserialized.carPoses.clear();
	if (unserialized.placementMode == RailTrainPlacementMode::OnLine)
	{
		unserialized.pose = RailTrainPose();
	}
	m_trains.push_back(unserialized);
	m_nextTrainId = std::max(m_nextTrainId, train.id + 1);
	return true;
}

bool RailTrainManager::deleteTrain(RailTrainId trainId)
{
	auto iter = std::remove_if(m_trains.begin(), m_trains.end(), [trainId](const RailTrain& train)
	{
		return train.id == trainId;
	});
	if (iter == m_trains.end())
	{
		return false;
	}
	m_trains.erase(iter, m_trains.end());
	return true;
}

bool RailTrainManager::assignLine(RailTrainId trainId, RailLineId lineId, const RailLineManager& lineManager)
{
	RailTrain* targetTrain = train(trainId);
	const RailLine* targetLine = lineManager.line(lineId);
	if (!targetTrain || !targetLine || !targetLine->isUsable)
	{
		return false;
	}

	targetTrain->lineId = lineId;
	targetTrain->placementMode = RailTrainPlacementMode::OnLine;
	targetTrain->headDistance = 0.0f;
	targetTrain->direction = 1;
	targetTrain->active = true;
	targetTrain->isManuallyStopped = false;
	targetTrain->isDwelling = false;
	targetTrain->dwellRemainingSeconds = 0.0f;
	targetTrain->dwellingStationId = InvalidRailStationId;
	targetTrain->dwellingPlatformId = InvalidRailPlatformId;
	targetTrain->lastStoppedStationId = InvalidRailStationId;
	targetTrain->lastStoppedPlatformId = InvalidRailPlatformId;
	targetTrain->lastStopDistance = -1.0f;
	return true;
}

void RailTrainManager::clearLineAssignment(RailLineId lineId)
{
	for (RailTrain& targetTrain : m_trains)
	{
		if (targetTrain.lineId != lineId)
		{
			continue;
		}
		targetTrain.lineId = InvalidRailLineId;
		targetTrain.placementMode = targetTrain.pose.valid ? RailTrainPlacementMode::Detached : RailTrainPlacementMode::Unplaced;
		targetTrain.active = false;
		targetTrain.isManuallyStopped = false;
		targetTrain.isDwelling = false;
	}
}

void RailTrainManager::update(float deltaSeconds, const RailLineManager& lineManager,
	const RailStationManager& stationManager)
{
	for (RailTrain& targetTrain : m_trains)
	{
		if (targetTrain.lineId == InvalidRailLineId)
		{
			continue;
		}

		const RailLine* targetLine = lineManager.line(targetTrain.lineId);
		if (!targetLine || !targetLine->isUsable || targetLine->totalLength <= 0.0001f)
		{
			targetTrain.active = false;
			targetTrain.isDwelling = false;
			continue;
		}

		if (targetTrain.isManuallyStopped)
		{
			continue;
		}

		if (targetTrain.isDwelling)
		{
			targetTrain.dwellRemainingSeconds -= deltaSeconds;
			if (targetTrain.dwellRemainingSeconds <= 0.0f)
			{
				finishDwelling(targetTrain);
			}
			continue;
		}

		if (!targetTrain.active)
		{
			continue;
		}

		if (targetTrain.lastStoppedStationId != InvalidRailStationId
			&& std::fabs(targetTrain.headDistance - targetTrain.lastStopDistance) > StopCooldownDistance)
		{
			targetTrain.lastStoppedStationId = InvalidRailStationId;
			targetTrain.lastStoppedPlatformId = InvalidRailPlatformId;
			targetTrain.lastStopDistance = -1.0f;
		}

		const float oldDistance = targetTrain.headDistance;
		float newDistance = targetTrain.headDistance + targetTrain.speed * deltaSeconds * static_cast<float>(targetTrain.direction);
		if (targetLine->isLoop)
		{
			while (newDistance < 0.0f)
			{
				newDistance += targetLine->totalLength;
			}
			while (newDistance > targetLine->totalLength)
			{
				newDistance -= targetLine->totalLength;
			}
		}
		else if (newDistance > targetLine->totalLength)
		{
			newDistance = targetLine->totalLength;
			targetTrain.direction = -1;
		}
		else if (newDistance < 0.0f)
		{
			newDistance = 0.0f;
			targetTrain.direction = 1;
		}

		StationStopCandidate stop = findNextStationStop(targetTrain, *targetLine, stationManager,
			oldDistance, newDistance);
		if (stop.valid)
		{
			targetTrain.headDistance = stop.distanceOnLine;
			startDwelling(targetTrain, stop);
		}
		else
		{
			targetTrain.headDistance = newDistance;
		}
	}
}

void RailTrainManager::updateWorldPoses(const RailNetwork& network, const RailLineManager& lineManager)
{
	for (RailTrain& targetTrain : m_trains)
	{
		if (targetTrain.placementMode != RailTrainPlacementMode::OnLine || targetTrain.lineId == InvalidRailLineId)
		{
			continue;
		}

		const RailLine* targetLine = lineManager.line(targetTrain.lineId);
		if (!targetLine || !targetLine->isUsable)
		{
			continue;
		}

		const int carCount = targetTrain.carriageCount + 1;
		if (static_cast<int>(targetTrain.carPoses.size()) != carCount)
		{
			targetTrain.carPoses.resize(carCount);
		}

		for (int i = 0; i < carCount; ++i)
		{
			float distance = targetTrain.headDistance - TrainCarSpacing * static_cast<float>(i) * static_cast<float>(targetTrain.direction);
			if (targetLine->isLoop)
			{
				while (distance < 0.0f)
				{
					distance += targetLine->totalLength;
				}
				while (distance > targetLine->totalLength)
				{
					distance -= targetLine->totalLength;
				}
			}
			else
			{
				distance = std::max(0.0f, std::min(distance, targetLine->totalLength));
			}

			RailLineSample sample = lineManager.sampleLine(network, *targetLine, distance);
			if (!sample.valid)
			{
				continue;
			}

			if (targetTrain.direction < 0)
			{
				sample.tangent = -sample.tangent;
			}

			targetTrain.carPoses[i].valid = true;
			targetTrain.carPoses[i].position = sample.position;
			targetTrain.carPoses[i].tangent = sample.tangent;
		}

		if (!targetTrain.carPoses.empty() && targetTrain.carPoses.front().valid)
		{
			targetTrain.pose = targetTrain.carPoses.front();
		}
	}
}

void RailTrainManager::refreshAfterLineRebuild(const RailLineManager& lineManager)
{
	for (RailTrain& targetTrain : m_trains)
	{
		const RailLine* targetLine = lineManager.line(targetTrain.lineId);
		if (!targetLine || !targetLine->isUsable)
		{
			targetTrain.active = false;
			targetTrain.isDwelling = false;
			continue;
		}
		clampTrainToLine(targetTrain, *targetLine);
	}
}

void RailTrainManager::clear()
{
	m_trains.clear();
	m_nextTrainId = 1;
}

RailTrain* RailTrainManager::train(RailTrainId trainId)
{
	for (RailTrain& candidate : m_trains)
	{
		if (candidate.id == trainId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const RailTrain* RailTrainManager::train(RailTrainId trainId) const
{
	for (const RailTrain& candidate : m_trains)
	{
		if (candidate.id == trainId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const std::vector<RailTrain>& RailTrainManager::trains() const
{
	return m_trains;
}

bool RailTrainManager::setTrainRunning(RailTrainId trainId, bool isRunning)
{
	RailTrain* targetTrain = train(trainId);
	if (!targetTrain)
	{
		return false;
	}

	targetTrain->isManuallyStopped = !isRunning;
	return true;
}

bool RailTrainManager::toggleTrainRunning(RailTrainId trainId)
{
	RailTrain* targetTrain = train(trainId);
	if (!targetTrain)
	{
		return false;
	}

	targetTrain->isManuallyStopped = !targetTrain->isManuallyStopped;
	return true;
}

void RailTrainManager::addTrainArrivedListener(std::function<void(const RailTrainEvent&)> listener)
{
	m_trainArrivedListeners.push_back(listener);
}

void RailTrainManager::addTrainDepartedListener(std::function<void(const RailTrainEvent&)> listener)
{
	m_trainDepartedListeners.push_back(listener);
}

void RailTrainManager::clampTrainToLine(RailTrain& train, const RailLine& line)
{
	if (line.totalLength <= 0.0001f)
	{
		train.headDistance = 0.0f;
		train.active = false;
		train.isDwelling = false;
		return;
	}
	train.headDistance = std::max(0.0f, std::min(train.headDistance, line.totalLength));
}

RailTrainManager::StationStopCandidate RailTrainManager::findNextStationStop(const RailTrain& train,
	const RailLine& line, const RailStationManager& stationManager, float oldDistance, float newDistance) const
{
	StationStopCandidate result;
	float bestTravelDistance = std::numeric_limits<float>::max();
	for (const RailLineControlPoint& controlPoint : line.controlPoints)
	{
		if (controlPoint.kind != RailLineControlPointKind::Station || !controlPoint.isResolved)
		{
			continue;
		}

		const RailPlatform* platform = stationManager.firstPlatform(controlPoint.stationId);
		if (!platform)
		{
			continue;
		}

		if (train.lastStoppedStationId == controlPoint.stationId
			&& train.lastStoppedPlatformId == platform->id
			&& std::fabs(oldDistance - controlPoint.distanceOnLine) <= StopCooldownDistance)
		{
			continue;
		}

		float travelDistance = std::numeric_limits<float>::max();
		if (line.isLoop)
		{
			if (train.direction >= 0)
			{
				travelDistance = controlPoint.distanceOnLine - oldDistance;
				if (travelDistance <= StopTriggerEpsilon)
				{
					travelDistance += line.totalLength;
				}
			}
			else
			{
				travelDistance = oldDistance - controlPoint.distanceOnLine;
				if (travelDistance <= StopTriggerEpsilon)
				{
					travelDistance += line.totalLength;
				}
			}
			const float movedDistance = train.direction >= 0 ? newDistance - oldDistance : oldDistance - newDistance;
			const float wrappedMovedDistance = movedDistance >= 0.0f ? movedDistance : movedDistance + line.totalLength;
			if (travelDistance > wrappedMovedDistance + StopTriggerEpsilon)
			{
				continue;
			}
		}
		else if (train.direction >= 0)
		{
			if (controlPoint.distanceOnLine <= oldDistance + StopTriggerEpsilon
				|| controlPoint.distanceOnLine > newDistance + StopTriggerEpsilon)
			{
				continue;
			}
			travelDistance = controlPoint.distanceOnLine - oldDistance;
		}
		else
		{
			if (controlPoint.distanceOnLine >= oldDistance - StopTriggerEpsilon
				|| controlPoint.distanceOnLine < newDistance - StopTriggerEpsilon)
			{
				continue;
			}
			travelDistance = oldDistance - controlPoint.distanceOnLine;
		}

		if (travelDistance < bestTravelDistance)
		{
			bestTravelDistance = travelDistance;
			result.valid = true;
			result.stationId = controlPoint.stationId;
			result.platformId = platform->id;
			result.distanceOnLine = controlPoint.distanceOnLine;
		}
	}
	return result;
}

void RailTrainManager::startDwelling(RailTrain& train, const StationStopCandidate& stop)
{
	train.isDwelling = true;
	train.dwellRemainingSeconds = StationDwellSeconds;
	train.dwellingStationId = stop.stationId;
	train.dwellingPlatformId = stop.platformId;
	train.lastStoppedStationId = stop.stationId;
	train.lastStoppedPlatformId = stop.platformId;
	train.lastStopDistance = stop.distanceOnLine;
	notifyArrived(train);
}

void RailTrainManager::finishDwelling(RailTrain& train)
{
	notifyDeparted(train);
	train.isDwelling = false;
	train.dwellRemainingSeconds = 0.0f;
	train.dwellingStationId = InvalidRailStationId;
	train.dwellingPlatformId = InvalidRailPlatformId;
}

void RailTrainManager::notifyArrived(const RailTrain& train)
{
	RailTrainEvent eventInfo;
	eventInfo.trainId = train.id;
	eventInfo.lineId = train.lineId;
	eventInfo.stationId = train.dwellingStationId;
	eventInfo.platformId = train.dwellingPlatformId;
	for (auto& listener : m_trainArrivedListeners)
	{
		if (listener)
		{
			listener(eventInfo);
		}
	}
}

void RailTrainManager::notifyDeparted(const RailTrain& train)
{
	RailTrainEvent eventInfo;
	eventInfo.trainId = train.id;
	eventInfo.lineId = train.lineId;
	eventInfo.stationId = train.dwellingStationId;
	eventInfo.platformId = train.dwellingPlatformId;
	for (auto& listener : m_trainDepartedListeners)
	{
		if (listener)
		{
			listener(eventInfo);
		}
	}
}

} // namespace tzw
