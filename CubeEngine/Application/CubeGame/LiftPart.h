#pragma once
#include "BearPart.h"
#include "GamePart.h"
namespace tzw
{
	class Island;
	class CubePrimitive;
	class CylinderPrimitive;
	class LiftPart : public GamePart
	{
	public:
		LiftPart();
		~LiftPart();
		Attachment * findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up) override;
		void initAttachments();
		Attachment * getAttachmentInfo(int index, vec3&pos, vec3&N, vec3&up);
		Attachment * getFirstAttachment() override;
		void liftUp(float val);
		void setEffectedIsland(std::string islandGroup);
		void setPos(vec3 v);
		bool isHit(Ray ray) override;
		Drawable3D* getNode() const override;
		void highLight() override;
		void unhighLight() override;
		void setIsVisible(bool isVisible);
		void setMaterial(Material * mat);
	public:
		float m_liftHeight;
		GamePartType getType() override;
		Attachment * m_attachment[6];
		std::string m_effectedIslandGroup;
	private:
		CubePrimitive * m_plaftormPart;
		CylinderPrimitive * m_pipePart;
		CubePrimitive * m_basePart;

	};
}
