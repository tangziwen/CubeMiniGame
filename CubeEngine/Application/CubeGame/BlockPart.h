#pragma once
#include "BearPart.h"
#include "GamePart.h"
namespace tzw
{
	class Island;
	struct Attachment
	{
		vec3 m_pos;
		vec3 m_normal;
		vec3 m_up;
		Attachment(vec3 thePos, vec3 n, vec3 f);
		Attachment();
	};
	class BlockPart : public GamePart
	{
	public:
		BlockPart();
		bool findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up) override;
		void attachTo(vec3 attachPosition, vec3 Normal, vec3 up) override;
		void initAttachments();
		void getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
	public:
		void cook();
		Attachment m_attachment[6];
		BearPart * m_bearPart[6];
		Island * m_parent;
	};
}
