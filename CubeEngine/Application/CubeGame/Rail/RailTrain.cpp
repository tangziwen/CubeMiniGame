#include "RailTrain.h"

#include <algorithm>

namespace tzw {

namespace
{
constexpr float TrainCarSpacing = 1.25f;
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
	}
}

void RailTrainManager::update(float deltaSeconds, const RailLineManager& lineManager)
{
	for (RailTrain& targetTrain : m_trains)
	{
		if (!targetTrain.active || targetTrain.lineId == InvalidRailLineId)
		{
			continue;
		}

		const RailLine* targetLine = lineManager.line(targetTrain.lineId);
		if (!targetLine || !targetLine->isUsable || targetLine->totalLength <= 0.0001f)
		{
			targetTrain.active = false;
			continue;
		}

		targetTrain.headDistance += targetTrain.speed * deltaSeconds * static_cast<float>(targetTrain.direction);
		if (targetLine->isLoop)
		{
			while (targetTrain.headDistance < 0.0f)
			{
				targetTrain.headDistance += targetLine->totalLength;
			}
			while (targetTrain.headDistance > targetLine->totalLength)
			{
				targetTrain.headDistance -= targetLine->totalLength;
			}
		}
		else if (targetTrain.headDistance > targetLine->totalLength)
		{
			targetTrain.headDistance = targetLine->totalLength;
			targetTrain.direction = -1;
		}
		else if (targetTrain.headDistance < 0.0f)
		{
			targetTrain.headDistance = 0.0f;
			targetTrain.direction = 1;
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

void RailTrainManager::clampTrainToLine(RailTrain& train, const RailLine& line)
{
	if (line.totalLength <= 0.0001f)
	{
		train.headDistance = 0.0f;
		train.active = false;
		return;
	}
	train.headDistance = std::max(0.0f, std::min(train.headDistance, line.totalLength));
}

} // namespace tzw
