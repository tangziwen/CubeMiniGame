#pragma once
#include "GameConstraint.h"
#include "GameNodeEditorNode.h"

namespace tzw
{
class vec3;
class Drawable3D;
class BlockPart;
struct Attachment;
class Physics6DofConstraint;
class SpringPart : public GameConstraint
{
public:
	SpringPart();
	Physics6DofConstraint * m_constrain;
	void enablePhysics(bool isEnable) override;
	void dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator) override;
	Attachment* getFirstAttachment() override;
	Attachment * getAttachment(int index) override;
	int getAttachmentCount() override;
	float getStiffness() const;
	float getDamping() const;
	void setStiffness(float stiffness);
	void setDamping(float damping);
	GameNodeEditorNode * getGraphNode() const;
	void generateName() override;
	virtual ~SpringPart();
	void drawInspect() override;
	bool isNeedDrawInspect() override;
	void updateConstraintState() override;
private:
	void findPiovtAndAxis(Attachment * attach, vec3 hingeDir, vec3 & pivot, vec3 & asix);
	GameNodeEditorNode * m_graphNode;
	float m_stiffness;
	float m_damping;

};
}
