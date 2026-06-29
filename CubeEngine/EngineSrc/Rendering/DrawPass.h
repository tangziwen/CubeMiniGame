#pragma once

#include <cstdint>
#include <string>

namespace tzw
{
enum DrawPassType : uint32_t
{
	Unset = 0,
	GBuffer = 1UL << 1,
	Transparent = 1UL << 2,
	AfterDepthClear = 1UL << 3,
	GUI = 1UL << 4,
	Shadow = 1UL << 5,
	DebugLayer = 1UL << 6,
	OutlineMask = 1UL << 7,
	All = 0xFFFFFFFFu
};

using DrawPassTypeMask = uint32_t;

DrawPassType parseDrawPassType(const std::string& name, DrawPassType defaultType);
const char* drawPassTypeToString(DrawPassType type);
}
