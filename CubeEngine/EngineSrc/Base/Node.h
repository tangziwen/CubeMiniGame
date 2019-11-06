#ifndef TZW_NODE_H
#define TZW_NODE_H
#include "../Math/vec3.h"
#include "../Math/vec2.h"
#include "../Math/Matrix44.h"
#include "../Math/Quaternion.h"
#include <string>
#include <vector>
#include <deque>
#include "Rendering/RenderCommand.h"
#include "../Action/ActionMgr.h"
#include <functional>

namespace tzw {
class Scene;
class Node : public ActionMgr
{
public:

	enum class NodeType
	{
		NormalNode,
		Drawable3D,
		Drawable,
	};

	Node();
	virtual ~Node();
	//
	static Node * create();
	virtual Matrix44 getTransform();
	virtual Matrix44 getLocalTransform();
	vec3 getPos() const;
	virtual void setPos(const vec3 &pos);
	void setPos(float x, float y, float z);
	virtual void submitDrawCmd(RenderCommand::RenderType passType);
	std::function<void (RenderCommand::RenderType)> onSubmitDrawCommand;
	virtual void logicUpdate(float dt);
	vec3 getRotateE();
	void setRotateE(const vec3 &rotate);
	void setRotateE(float x,float y,float z);
	vec3 getScale() const;
	void setScale(const vec3 &scale);
	void setScale(float x,float y,float z);
	Matrix44 getTranslationMatrix();
	Matrix44 getRotationMatrix();
	Matrix44 getScalingMatrix();
	void addChild(Node * node, bool isNeedSort = true);
	Node *getParent() const;
	std::string getName() const;
	Node *getChildByName(const std::string &name);
	Node *getChildByTag(unsigned int tag);
	Node * getChildByIndex(size_t index);
	void setName(const std::string &name);
	void setPos2D(float x,float y);
	void setPos2D(vec2 pos);
	vec2 getPos2D();
	vec3 getWorldPos();
	vec2 getWorldPos2D();
	bool getNeedToUpdate() const;
	void setNeedToUpdate(bool needToUpdate);
	virtual NodeType getNodeType();
	bool getIsVisible() const;
	virtual void setIsVisible(bool isVisible);
	void purgeAllChildren();
	virtual bool getIsAccpectOcTtree() const;
	void setIsAccpectOcTtree(bool isAccpectOCTtree);
	void cacheTransform();
	virtual void visit(RenderCommand::RenderType passType);
	virtual void reCache();
	bool getIsValid() const;
	void setIsValid(bool isValid);
	void detachChild(Node * node);
	void removeFromParent();
	void setParent(Node *parent);
	vec3 getForward();
	vec3 getUp();
	int getLocalPiority() const;
	void setLocalPiority(int zOrder);
	void sortChildren();
	Scene *getScene() const;
	void setScene(Scene *scene);
	Quaternion getRotateQ() const;
	virtual void setRotateQ(const Quaternion &rotateQ);
	unsigned int getGlobalPiority() const;
	void setGlobalPiority(unsigned int globalPiority);
	size_t getChildrenAmount();
	unsigned int getTag() const;
	void setTag(unsigned int tag);
	virtual unsigned int getTypeId();
protected:
	bool m_isVisible;
	std::deque<Node *> m_children;
	bool m_isValid;
	Scene* m_scene;
	vec3 m_scale;
	Quaternion m_rotateQ;
	vec3 m_rotateE;
	vec3 m_pos;
	bool m_needToUpdate;
	Matrix44 m_worldTransformCache;
	int m_localPiority;
	unsigned int m_globalPiority;
	bool m_isAccpectOCTtree;
	std::string m_name;
	Node * m_parent;
	unsigned int m_tag;
	void removeAllChildrenR();
};

} // namespace tzw

#endif // TZW_NODE_H
