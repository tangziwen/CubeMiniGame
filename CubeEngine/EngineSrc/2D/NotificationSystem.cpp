#include "NotificationSystem.h"

#include "2D/LabelNew.h"
#include "2D/RetainedUISystem.h"
#include "2D/Sprite.h"
#include "Base/Node.h"
#include "Engine/Engine.h"
#include "Event/EventMgr.h"

#include <algorithm>

namespace tzw
{
namespace
{
const float ToastMargin = 18.0f;
const float ToastPaddingX = 14.0f;
const float ToastGap = 8.0f;
const float ToastMinWidth = 240.0f;
const float ToastMaxWidth = 420.0f;
const float ToastMinHeight = 46.0f;
const float ToastLifeLiftRatio = 0.1f;

vec4 colorForLevel(NotificationLevel level)
{
	switch (level)
	{
	case NotificationLevel::Warning:
		return vec4::fromRGB(190, 130, 45, 230);
	case NotificationLevel::Error:
		return vec4::fromRGB(190, 70, 70, 230);
	case NotificationLevel::Info:
	default:
		return vec4::fromRGB(55, 70, 85, 225);
	}
}
}

class NotificationToastView
{
public:
	NotificationToastView(Node* parent, const std::string& text, NotificationLevel level, float seconds)
		: m_totalSeconds(seconds),
		  m_remainingSeconds(seconds)
	{
		m_root = Node::create();
		m_root->setName("notification_toast");
		m_root->setLocalPriority(10000);
		m_root->setNodeEventPriority(10000);
		parent->addChild(m_root);

		m_label = LabelNew::create(text);
		const vec2 labelSize = m_label->getContentSize();
		m_size.x = std::min(std::max(labelSize.x + ToastPaddingX * 2.0f, ToastMinWidth), ToastMaxWidth);
		m_size.y = std::max(labelSize.y + 18.0f, ToastMinHeight);

		m_background = Sprite::createWithColor(colorForLevel(level), m_size);
		m_background->setTouchEnable(false);
		m_background->setNodeEventPriority(10001);
		m_background->setPos2D(m_size.x * 0.5f, m_size.y * 0.5f);
		m_root->addChild(m_background);

		m_label->setNodeEventPriority(10002);
		m_label->setPos2D(ToastPaddingX, m_size.y * 0.5f + 7.0f);
		m_root->addChild(m_label);
	}

	~NotificationToastView()
	{
		if (m_background)
		{
			if (m_background->getParent())
			{
				m_background->getParent()->detachChild(m_background);
				m_background->setParent(nullptr);
			}
			delete m_background;
			m_background = nullptr;
		}
		if (m_label)
		{
			if (m_label->getParent())
			{
				m_label->getParent()->detachChild(m_label);
				m_label->setParent(nullptr);
			}
			delete m_label;
			m_label = nullptr;
		}
		if (m_root)
		{
			if (m_root->getParent())
			{
				m_root->getParent()->detachChild(m_root);
				m_root->setParent(nullptr);
			}
			delete m_root;
			m_root = nullptr;
		}
	}

	void update(float deltaSeconds)
	{
		m_remainingSeconds -= deltaSeconds;
	}

	bool isExpired() const
	{
		return m_remainingSeconds <= 0.0f;
	}

	void setStackIndex(int index, const vec2& screenSize)
	{
		if (!m_root)
		{
			return;
		}
		const float x = screenSize.x - m_size.x - ToastMargin;
		float ageRatio = 0.0f;
		if (m_totalSeconds > 0.0f)
		{
			ageRatio = 1.0f - std::max(m_remainingSeconds, 0.0f) / m_totalSeconds;
		}
		const float lifeLift = screenSize.y * ToastLifeLiftRatio;
		const float y = ToastMargin + index * (m_size.y + ToastGap) + ageRatio * lifeLift;
		m_root->setPos2D(x, y);
	}

private:
	Node* m_root = nullptr;
	Sprite* m_background = nullptr;
	LabelNew* m_label = nullptr;
	vec2 m_size = vec2(ToastMinWidth, ToastMinHeight);
	float m_totalSeconds = 0.0f;
	float m_remainingSeconds = 0.0f;
};

NotificationSystem::NotificationSystem()
{
	setIsSwallow(false);
	RetainedUISystem::shared();
	EventMgr::shared()->addStandaloneEventListener(this);
}

NotificationSystem::~NotificationSystem() = default;

void NotificationSystem::push(const std::string& text, NotificationLevel level, float seconds)
{
	Node* root = RetainedUISystem::shared()->guiRoot();
	if (!root)
	{
		return;
	}

	Scene* scene = RetainedUISystem::shared()->guiScene();
	if (scene != m_guiScene)
	{
		clearToasts();
		m_guiScene = scene;
	}

	m_toasts.emplace_back(new NotificationToastView(root, text, level, seconds));
	layoutToasts();
}

void NotificationSystem::onFrameUpdate(float delta)
{
	Node* root = RetainedUISystem::shared()->guiRoot();
	if (!root)
	{
		clearToasts();
		m_guiScene = nullptr;
		return;
	}

	Scene* scene = RetainedUISystem::shared()->guiScene();
	if (scene != m_guiScene)
	{
		clearToasts();
		m_guiScene = scene;
	}

	for (auto& toast : m_toasts)
	{
		toast->update(delta);
	}

	m_toasts.erase(std::remove_if(m_toasts.begin(), m_toasts.end(),
		[](const std::unique_ptr<NotificationToastView>& toast)
	{
		return toast->isExpired();
	}), m_toasts.end());

	layoutToasts();
}

void NotificationSystem::clearToasts()
{
	m_toasts.clear();
}

void NotificationSystem::layoutToasts()
{
	const vec2 screenSize = Engine::shared()->winSize();
	for (int i = 0; i < static_cast<int>(m_toasts.size()); ++i)
	{
		const int stackIndex = static_cast<int>(m_toasts.size()) - 1 - i;
		m_toasts[i]->setStackIndex(stackIndex, screenSize);
	}
}
}
