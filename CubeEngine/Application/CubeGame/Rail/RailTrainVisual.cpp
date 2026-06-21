#include "RailTrainVisual.h"

#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "EngineSrc/Base/Node.h"
#include "EngineSrc/Math/Quaternion.h"
#include "EngineSrc/Math/vec4.h"

#include <algorithm>

namespace tzw {

namespace
{
vec3 safeNormalized(const vec3& value, const vec3& fallback)
{
	const float length = value.length();
	if (length <= 0.0001f)
	{
		return fallback;
	}
	return value / length;
}

Quaternion rotationForTangent(const vec3& tangent)
{
	const vec3 forward = safeNormalized(vec3(tangent.x, 0.0f, tangent.z), vec3(0.0f, 0.0f, -1.0f));
	const vec3 up(0.0f, 1.0f, 0.0f);
	const vec3 right = safeNormalized(vec3::CrossProduct(up, forward), vec3(1.0f, 0.0f, 0.0f));
	Quaternion rotation;
	rotation.fromAxises(right, up, forward);
	rotation.normalize();
	return rotation;
}

bool containsTrain(const RailTrainManager& trainManager, RailTrainId trainId)
{
	for (const RailTrain& train : trainManager.trains())
	{
		if (train.id == trainId)
		{
			return true;
		}
	}
	return false;
}
}

RailTrainVisualSet::~RailTrainVisualSet()
{
	clear();
}

void RailTrainVisualSet::sync(Node* sceneRoot, const RailNetwork& network, const RailLineManager& lineManager,
	const RailTrainManager& trainManager)
{
	(void)network;
	(void)lineManager;

	if (!sceneRoot)
	{
		return;
	}
	ensureVisualRoot(sceneRoot);
	hideUnusedTrains(trainManager);

	for (const RailTrain& train : trainManager.trains())
	{
		TrainVisual& visual = ensureTrainVisual(train.id);
		const int carCount = train.carriageCount + 1;
		ensureCarCount(visual, carCount);

		for (int i = 0; i < carCount; ++i)
		{
			CubePrimitive* car = visual.cars[i];
			if (i >= static_cast<int>(train.carPoses.size()) || !train.carPoses[i].valid)
			{
				car->setIsVisible(false);
				continue;
			}

			car->setIsVisible(true);
			car->setPos(train.carPoses[i].position + vec3(0.0f, 0.35f, 0.0f));
			car->setRotateQ(rotationForTangent(train.carPoses[i].tangent));
		}
		for (size_t i = carCount; i < visual.cars.size(); ++i)
		{
			visual.cars[i]->setIsVisible(false);
		}
	}
}

void RailTrainVisualSet::clear()
{
	m_trainVisuals.clear();
	if (m_visualRoot)
	{
		if (m_visualRoot->getParent())
		{
			m_visualRoot->removeFromParent();
		}
		delete m_visualRoot;
		m_visualRoot = nullptr;
	}
}

void RailTrainVisualSet::ensureVisualRoot(Node* sceneRoot)
{
	if (!m_visualRoot)
	{
		m_visualRoot = new Node();
		m_visualRoot->setName("RailTrainVisualRoot");
	}
	if (!m_visualRoot->getParent())
	{
		sceneRoot->addChild(m_visualRoot, false);
	}
}

RailTrainVisualSet::TrainVisual& RailTrainVisualSet::ensureTrainVisual(RailTrainId trainId)
{
	for (TrainVisual& visual : m_trainVisuals)
	{
		if (visual.trainId == trainId)
		{
			return visual;
		}
	}

	TrainVisual visual;
	visual.trainId = trainId;
	m_trainVisuals.push_back(visual);
	return m_trainVisuals.back();
}

void RailTrainVisualSet::ensureCarCount(TrainVisual& visual, int carCount)
{
	while (static_cast<int>(visual.cars.size()) < carCount)
	{
		const bool isHead = visual.cars.empty();
		auto car = new CubePrimitive(0.55f, 0.55f, isHead ? 0.95f : 0.85f, true);
		car->setIsAccpectOcTtree(false);
		car->setIsHitable(false);
		car->setColor(isHead ? vec4::fromRGB(220, 70, 60) : vec4::fromRGB(70, 120, 220));
		m_visualRoot->addChild(car, false);
		visual.cars.push_back(car);
	}
}

void RailTrainVisualSet::hideUnusedTrains(const RailTrainManager& trainManager)
{
	for (TrainVisual& visual : m_trainVisuals)
	{
		if (containsTrain(trainManager, visual.trainId))
		{
			continue;
		}
		for (CubePrimitive* car : visual.cars)
		{
			car->setIsVisible(false);
		}
	}
}

} // namespace tzw
