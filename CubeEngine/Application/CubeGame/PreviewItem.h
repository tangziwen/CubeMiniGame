#pragma once
#include "Math/vec3.h"
#include "GameItem.h"
#include "3D/Primitive/SpherePrimitive.h"

namespace tzw
{
	class GamePart;
	class Island;
	class PreviewItem
	{
	public:
		PreviewItem();
		GamePart* getPreviewPart() const;
		void setPreviewPart(GamePart* const previewPart);
		Island* getPreviewIsland() const;
		void setPreviewIsland(Island* const previewIsland);
		float getPreviewAngle() const;
		void setPreviewAngle(const float previewAngle);
		void setPreviewItem(GameItem * item);
		void handlePreview(GameItem * currSelected, vec3 camPos, vec3 playerForwardDir);
		void switchAttachment();
		int getCurrAttachment();
	private:
		void hidePreviewPart();
		void showPreviewPart();
		int m_currAttachment;
		float m_previewAngle;
		GamePart * m_previewPart;
		Island * m_previewIsland;
		SpherePrimitive * m_sphere;
	};
}
