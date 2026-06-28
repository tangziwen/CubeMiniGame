#pragma once

#include <stdint.h>

namespace tzw
{
enum class RenderViewType : uint32_t
{
	Scene = 1UL << 0,
	Shadow = 1UL << 1,
};

inline uint32_t renderViewTypeToMask(RenderViewType viewType)
{
	return static_cast<uint32_t>(viewType);
}
}
