#pragma once

#include "Engine/EngineDef.h"
#include "Event/Event.h"

#include <memory>
#include <string>
#include <vector>

namespace tzw
{
class NotificationToastView;
class Scene;

enum class NotificationLevel
{
	Info,
	Warning,
	Error,
};

class NotificationSystem : public Singleton<NotificationSystem>, public EventListener
{
public:
	NotificationSystem();
	~NotificationSystem();

	void push(const std::string& text, NotificationLevel level = NotificationLevel::Info, float seconds = 3.0f);
	void onFrameUpdate(float delta) override;

private:
	void clearToasts();
	void layoutToasts();

	Scene* m_guiScene = nullptr;
	std::vector<std::unique_ptr<NotificationToastView>> m_toasts;
};
}
