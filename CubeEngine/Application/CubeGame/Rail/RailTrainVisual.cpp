#include "RailTrainVisual.h"

#include "CubeGame/Rail/RailInspectTarget.h"
#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "EngineSrc/Base/Node.h"
#include "EngineSrc/Math/Quaternion.h"
#include "EngineSrc/Math/vec4.h"

#include <algorithm>
#include <memory>

namespace tzw {

namespace
{
constexpr int TrainPickPriority = 30;
const vec3 TrainCarPickHalfExtents(0.38f, 0.58f, 0.38f);

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

AABB makeCenteredBounds(const vec3& halfExtents)
{
	AABB bounds;
	bounds.update(-halfExtents);
	bounds.update(halfExtents);
	return bounds;
}

Matrix44 makePickTransform(const vec3& center, const Quaternion& rotation)
{
	Matrix44 translation;
	translation.setTranslate(center);
	Matrix44 rotationMatrix;
	rotationMatrix.setRotation(rotation);
	return translation * rotationMatrix;
}

std::unique_ptr<PickShape> makeTrainCarPickShape(const vec3& center, const Quaternion& rotation)
{
	return std::make_unique<BoxPickShape>(makeCenteredBounds(TrainCarPickHalfExtents),
		makePickTransform(center, rotation));
}

PickPayload trainPickPayload(RailTrainId trainId)
{
	PickPayload payload;
	payload.type = static_cast<int>(RailInspectTargetKind::Train);
	payload.id0 = trainId;
	return payload;
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
				setCarPickTargetEnabled(visual, i, false);
				continue;
			}

			car->setIsVisible(true);
			const vec3 carCenter = train.carPoses[i].position + vec3(0.0f, 0.35f, 0.0f);
			car->setPos(carCenter);
			Quaternion rotation;
			rotation.fromDirection(vec3(train.carPoses[i].tangent.x, 0.0f, train.carPoses[i].tangent.z));
			car->setRotateQ(rotation);
			syncCarPickTarget(visual, i, carCenter, rotation);
		}
		applyOutline(visual);
		for (size_t i = carCount; i < visual.cars.size(); ++i)
		{
			visual.cars[i]->setIsVisible(false);
			setCarPickTargetEnabled(visual, static_cast<int>(i), false);
		}
	}
}

void RailTrainVisualSet::clear()
{
	for (TrainVisual& visual : m_trainVisuals)
	{
		clearPickTargets(visual);
	}
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

void RailTrainVisualSet::setTrainOutline(RailTrainId trainId, bool enabled, vec4 color)
{
	TrainVisual* existingVisual = nullptr;
	for (TrainVisual& visual : m_trainVisuals)
	{
		if (visual.trainId == trainId)
		{
			existingVisual = &visual;
			break;
		}
	}
	if (!enabled && !existingVisual)
	{
		return;
	}

	TrainVisual& visual = existingVisual ? *existingVisual : ensureTrainVisual(trainId);
	visual.outlineEnabled = enabled;
	visual.outlineColor = color;
	applyOutline(visual);
}

void RailTrainVisualSet::clearOutlines()
{
	for (TrainVisual& visual : m_trainVisuals)
	{
		visual.outlineEnabled = false;
		applyOutline(visual);
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
		car->setOutlineEnabled(visual.outlineEnabled);
		car->setOutlineColor(visual.outlineColor);
		m_visualRoot->addChild(car, false);
		visual.cars.push_back(car);
		visual.carPickHandles.push_back(PickHandle());
	}
}

void RailTrainVisualSet::applyOutline(TrainVisual& visual)
{
	for (CubePrimitive* car : visual.cars)
	{
		car->setOutlineEnabled(visual.outlineEnabled);
		car->setOutlineColor(visual.outlineColor);
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
		clearPickTargets(visual);
		for (CubePrimitive* car : visual.cars)
		{
			car->setIsVisible(false);
			car->setOutlineEnabled(false);
		}
	}
}

void RailTrainVisualSet::syncCarPickTarget(TrainVisual& visual, int carIndex, const vec3& center,
	const Quaternion& rotation)
{
	if (carIndex < 0 || carIndex >= static_cast<int>(visual.carPickHandles.size()))
	{
		return;
	}

	std::unique_ptr<PickShape> shape = makeTrainCarPickShape(center, rotation);
	PickHandle& handle = visual.carPickHandles[carIndex];
	if (!handle.isValid())
	{
		PickTargetDesc desc;
		desc.category = RailInspectPickCategory;
		desc.priority = TrainPickPriority;
		desc.owner = this;
		desc.payload = trainPickPayload(visual.trainId);
		desc.enabled = true;
		desc.shape = std::move(shape);
		handle = ScenePickingSystem::shared()->registerTarget(std::move(desc));
		return;
	}

	ScenePickingSystem::shared()->updateTargetShape(handle, std::move(shape));
	ScenePickingSystem::shared()->setTargetEnabled(handle, true);
}

void RailTrainVisualSet::setCarPickTargetEnabled(TrainVisual& visual, int carIndex, bool enabled)
{
	if (carIndex < 0 || carIndex >= static_cast<int>(visual.carPickHandles.size()))
	{
		return;
	}
	PickHandle& handle = visual.carPickHandles[carIndex];
	if (handle.isValid())
	{
		ScenePickingSystem::shared()->setTargetEnabled(handle, enabled);
	}
}

void RailTrainVisualSet::clearPickTargets(TrainVisual& visual)
{
	for (PickHandle& handle : visual.carPickHandles)
	{
		if (!handle.isValid())
		{
			continue;
		}
		ScenePickingSystem::shared()->unregisterTarget(handle);
		handle = PickHandle();
	}
}

} // namespace tzw
