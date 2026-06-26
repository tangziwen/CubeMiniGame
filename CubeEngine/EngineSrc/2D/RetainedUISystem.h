#pragma once

#include "Engine/EngineDef.h"
#include "Event/Event.h"

#include <string>

namespace tzw
{
class Node;
class Scene;

class RetainedUISystem : public Singleton<RetainedUISystem>, public EventListener
{
public:
	RetainedUISystem();

	Node* root();
	Node* guiRoot();
	Scene* guiScene() const;
	void add(Node* node, bool sort = true);
	Node* layer(const std::string& name);
	Node* findOrCreateLayer(const std::string& name, int localPriority);
	void addToLayer(const std::string& name, int localPriority, Node* node, bool sort = true);
	void onFrameUpdate(float delta) override;

private:
	Node* findOrCreateGuiRoot(Scene* scene);

	Scene* m_guiScene = nullptr;
	Node* m_guiRoot = nullptr;
};
}
