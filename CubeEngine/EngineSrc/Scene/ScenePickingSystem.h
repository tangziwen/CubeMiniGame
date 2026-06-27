#pragma once

#include "Base/Singleton.h"
#include "Math/AABB.h"
#include "Math/Matrix44.h"
#include "Math/Ray.h"
#include "Math/vec3.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace tzw {

struct PickHit
{
	bool hit = false;
	float distance = 0.0f;
	vec3 worldPoint;
};

class PickShape
{
public:
	virtual ~PickShape() = default;
	virtual bool hit(const Ray& worldRay, PickHit& outHit) const = 0;
};

class BoxPickShape : public PickShape
{
public:
	BoxPickShape(const AABB& localBounds, const Matrix44& worldTransform);
	bool hit(const Ray& worldRay, PickHit& outHit) const override;

private:
	AABB m_localBounds;
	Matrix44 m_worldTransform;
};

struct PickPayload
{
	int type = 0;
	int id0 = 0;
	int id1 = 0;
	void* userData = nullptr;
};

struct PickHandle
{
	int index = -1;
	unsigned int generation = 0;

	bool isValid() const;
};

struct PickTargetDesc
{
	uint32_t category = 0;
	int priority = 0;
	void* owner = nullptr;
	PickPayload payload;
	bool enabled = true;
	std::unique_ptr<PickShape> shape;
};

struct PickResult
{
	PickHandle handle;
	uint32_t category = 0;
	int priority = 0;
	void* owner = nullptr;
	PickPayload payload;
	float distance = 0.0f;
	vec3 worldPoint;
};

class ScenePickingSystem : public Singleton<ScenePickingSystem>
{
public:
	PickHandle registerTarget(PickTargetDesc desc);
	void unregisterTarget(PickHandle handle);
	void setTargetEnabled(PickHandle handle, bool enabled);
	void updateTargetShape(PickHandle handle, std::unique_ptr<PickShape> shape);
	bool pick(const Ray& ray, uint32_t categoryMask, PickResult& outResult) const;

	static bool makeCursorRay(Ray& outRay, float* outMaxDistance = nullptr);

private:
	struct PickTarget
	{
		unsigned int generation = 1;
		uint32_t category = 0;
		int priority = 0;
		void* owner = nullptr;
		PickPayload payload;
		bool enabled = false;
		std::unique_ptr<PickShape> shape;
	};

	bool isLiveHandle(PickHandle handle) const;

	std::vector<PickTarget> m_targets;
};

} // namespace tzw
