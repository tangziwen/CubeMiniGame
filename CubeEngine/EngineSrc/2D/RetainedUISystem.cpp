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
const char* DefaultLayerName = "default";
const char* OverlayLayerName = "overlay";
const char* ModalLayerName = "modal";
const char* DebugLayerName = "debug";
constexpr int GuiRootPriority = 10000;
constexpr int DefaultLayerPriority = 0;
constexpr int OverlayLayerPriority = 100;
constexpr int ModalLayerPriority = 200;
constexpr int DebugLayerPriority = 300;

Node* findOrCreateChildLayer(Node* root, const std::string& name, int localPriority)
{
	if (!root)
	{
		return nullptr;
	}

	Node* layerNode = root->getChildByName(name);
	if (layerNode)
	{
		layerNode->setLocalPriority(localPriority);
		return layerNode;
	}

	layerNode = Node::create();
	layerNode->setName(name);
	layerNode->setLocalPriority(localPriority);
	root->addChild(layerNode);
	return layerNode;
}

void ensureDefaultLayers(Node* root)
{
	findOrCreateChildLayer(root, DefaultLayerName, DefaultLayerPriority);
	findOrCreateChildLayer(root, OverlayLayerName, OverlayLayerPriority);
	findOrCreateChildLayer(root, ModalLayerName, ModalLayerPriority);
	findOrCreateChildLayer(root, DebugLayerName, DebugLayerPriority);
}
}

RetainedUISystem::RetainedUISystem()
{
	setIsSwallow(false);
	EventMgr::shared()->addStandaloneEventListener(this);
}

Node* RetainedUISystem::root()
{
	return guiRoot();
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
	ensureDefaultLayers(m_guiRoot);
	return m_guiRoot;
}

void RetainedUISystem::add(Node* node, bool sort)
{
	addToLayer(DefaultLayerName, DefaultLayerPriority, node, sort);
}

Node* RetainedUISystem::layer(const std::string& name)
{
	Node* rootNode = guiRoot();
	if (!rootNode)
	{
		return nullptr;
	}
	return rootNode->getChildByName(name);
}

Node* RetainedUISystem::findOrCreateLayer(const std::string& name, int localPriority)
{
	Node* rootNode = guiRoot();
	if (!rootNode)
	{
		return nullptr;
	}

	return findOrCreateChildLayer(rootNode, name, localPriority);
}

void RetainedUISystem::addToLayer(const std::string& name, int localPriority, Node* node, bool sort)
{
	if (!node)
	{
		return;
	}

	Node* layerNode = findOrCreateLayer(name, localPriority);
	if (!layerNode || node->getParent() == layerNode)
	{
		return;
	}
	if (node->getParent())
	{
		return;
	}
	layerNode->addChild(node, sort);
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
	root->setLocalPriority(GuiRootPriority);
	scene->addNode(root);
	return root;
}
}
