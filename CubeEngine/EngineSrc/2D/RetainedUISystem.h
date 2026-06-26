#pragma once

#include "Engine/EngineDef.h"
#include "Event/Event.h"

namespace tzw
{
class Node;
class Scene;

class RetainedUISystem : public Singleton<RetainedUISystem>, public EventListener
{
public:
	RetainedUISystem();

	Node* guiRoot();
	Scene* guiScene() const;
	void onFrameUpdate(float delta) override;

private:
	Node* findOrCreateGuiRoot(Scene* scene);

	Scene* m_guiScene = nullptr;
	Node* m_guiRoot = nullptr;
};
}
