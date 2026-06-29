#include "RailLineVisualStyle.h"

#include <algorithm>

namespace tzw {

namespace
{
const vec4 LinePalette[] = {
	vec4::fromRGB(72, 190, 220),
	vec4::fromRGB(236, 111, 86),
	vec4::fromRGB(118, 201, 96),
	vec4::fromRGB(236, 188, 72),
	vec4::fromRGB(178, 126, 232),
	vec4::fromRGB(232, 120, 185),
	vec4::fromRGB(92, 142, 238),
	vec4::fromRGB(232, 150, 72),
};

vec4 brighten(const vec4& color)
{
	return vec4(
		std::min(color.x * 1.25f, 1.0f),
		std::min(color.y * 1.25f, 1.0f),
		std::min(color.z * 1.25f, 1.0f),
		color.w);
}
}

vec4 railLineColor(RailLineId lineId)
{
	const int paletteSize = static_cast<int>(sizeof(LinePalette) / sizeof(LinePalette[0]));
	const int index = lineId <= 0 ? 0 : (lineId - 1) % paletteSize;
	return LinePalette[index];
}

vec4 railLineSelectedColor(RailLineId lineId)
{
	return brighten(railLineColor(lineId));
}

} // namespace tzw
