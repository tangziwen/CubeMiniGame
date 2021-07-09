#pragma once
#include "../EngineSrc/Math/vec3.h"
#include <cstdint>

namespace tzw
{
struct FPGunData
{
	FPGunData() = default;
	vec3 m_adsPos;
	vec3 m_hipPos;
	uint16_t m_magCapacity;
	uint16_t m_preserveAmmoCapacity;
	uint16_t m_damage;
	float m_scale;
	vec3 m_rotateE;
	std::string m_filePath;
	bool m_isAllowADS;
};
}
