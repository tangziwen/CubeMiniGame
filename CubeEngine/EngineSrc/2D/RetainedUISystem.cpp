#include "RetainedUISystem.h"

#include "Base/Node.h"
#include "Event/EventMgr.h"
#include "Scene/Scene.h"
#include "Scene/SceneMgr.h"

namespace tzw
{
namespace
{
const char* GuiRootName = "gui_root";
}

RetainedUISystem::RetainedUISystem()
{
	setIsSwallow(false);
	EventMgr::shared()->addFixedPiorityListener(this);
}

Node* RetainedUISystem::guiRoot()
{
	Scene* scene = g_GetCurrScene();
	if (!scene)
	{
		m_guiScene = nullptr;
		m_guiRoot = nullptr;
		return nullptr;
	}

	if (scene != m_guiScene || !m_guiRoot || m_guiRoot->getParent() != scene->root())
	{
		m_guiScene = scene;
		m_guiRoot = findOrCreateGuiRoot(scene);
	}
	return m_guiRoot;
}

Scene* RetainedUISystem::guiScene() const
{
	return m_guiScene;
}

void RetainedUISystem::onFrameUpdate(float)
{
	guiRoot();
}

Node* RetainedUISystem::findOrCreateGuiRoot(Scene* scene)
{
	if (!scene)
	{
		return nullptr;
	}

	Node* root = scene->root()->getChildByName(GuiRootName);
	if (root)
	{
		return root;
	}

	root = Node::create();
	root->setName(GuiRootName);
	root->setLocalPiority(10000);
	scene->addNode(root);
	return root;
}
}
