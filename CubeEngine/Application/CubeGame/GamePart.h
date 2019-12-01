#pragma once
#include "Attachment.h"
#include "Base/GuidObj.h"
#include "Collision/PhysicsShape.h"
#include "Interface/Drawable3D.h"
#include "Math/Ray.h"
#include "rapidjson/document.h"
#include "GameNodeEditorNode.h"


namespace tzw {
	class PartSurface;
	class GamePart;
class BearPart;
class Island;
struct GameItem;
class GamePart : public GuidObj
{
public:
	virtual Drawable3D* getNode() const;
	virtual void setNode(Drawable3D* node);
	virtual PhysicsShape* getShape();
	virtual Attachment* findProperAttachPoint(Ray ray,
	                                        vec3& attachPosition,
	                                        vec3& Normal,
	                                        vec3& up);
	virtual void setSurface(vec3 color, PartSurface * surface);
	virtual void attachTo(Attachment* attach, float degree, int attachMentIndex);
	virtual Matrix44 attachToOtherIslandByAlterSelfPart(Attachment* attach, int attachmentIndex);
	virtual Matrix44 attachToOtherIslandByAlterSelfIsland(
	Attachment* attach,
	Attachment* ownAttachment, float degree);
	virtual Matrix44 adjustToOtherIslandByAlterSelfIsland(Attachment* attach,
	                                                    Attachment* selfAttach, float degree);
	virtual Matrix44 adjustToOtherIslandByAlterSelfPart(Attachment* attach,
	                                     Attachment* selfAttach, float degree);
	virtual Attachment* getFirstAttachment();
	virtual Attachment* getBottomAttachment();
	virtual Attachment* getTopAttachment();
	virtual Attachment* getAttachment(int index);
	virtual int getAttachmentCount();
	virtual Attachment* getAttachmentInfo(int index,
	                                    vec3& pos,
	                                    vec3& N,
	                                    vec3& up);
	Island* m_parent;
	GamePart();
	virtual ~GamePart();
	virtual GamePartType getType();
	virtual float getMass();
	virtual void load(rapidjson::Value& partData);
	virtual void dump(rapidjson::Value& partDocObj,
	                rapidjson::Document::AllocatorType& allocator);

	virtual bool isConstraint();
	virtual vec3 getWorldPos();
	std::string getName() const;
	virtual void setName(std::string newName);
	virtual void generateName();
	std::string genShortName();
	virtual void highLight();
	virtual void unhighLight();
	virtual void use();
	virtual void dumpAttach(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator);
	virtual void loadAttach(rapidjson::Value& partDocObj);
	void addAttachment(Attachment * newAttach);
	virtual bool isHit(Ray ray);
	virtual void drawInspect();
	virtual bool isNeedDrawInspect();
	virtual bool drawInspectNameEdit();
	void initFromItemName(std::string itemName);
	void initFromItem(GameItem * item);
	virtual GameNodeEditorNode * getEditorNode();
	int getPrettyAttach(Attachment * otherAttach, int attachOffset);
	int findAttachByLocale(std::string locale);
protected:
	std::vector<Attachment * > m_attachment;
	std::string m_name;
	PhysicsShape* m_shape;
	Drawable3D* m_node;
	GameItem * m_item;
	PartSurface * m_partSurface;
public:
	GameItem* getItem() const;
	void setItem(GameItem* const item);
};
}
