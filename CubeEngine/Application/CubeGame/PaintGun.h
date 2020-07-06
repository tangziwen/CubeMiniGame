#pragma once
#include "Math/vec3.h"
#include "PartSurface.h"

namespace tzw
{
	class GamePart;
	struct PaintGun
	{
		PaintGun();
		void paint(GamePart * part);
		vec3 color;
		PartSurface * m_surface;
		int m_matIndex;
		bool m_enableSurfaceChanged;
	};
}
