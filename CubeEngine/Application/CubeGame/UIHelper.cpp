#include "BuildingSystem.h"
#include "Base/GuidMgr.h"
#include "Chunk.h"
#include "Utility/file/Tfile.h"
#include "rapidjson/filewritestream.h"
#include <algorithm>
#include <utility>

#include "UIHelper.h"
#include "Action/MoveBy.h"
#include "Action/ActionSequence.h"
#include "Action/ActionRemoveMySelf.h"

namespace tzw
{
	TZW_SINGLETON_IMPL(UIHelper);

	UIHelper::UIHelper()
	{
		
	}

	void UIHelper::showFloatTips(std::string str, float duration, float disappearDuration)
	{
		auto label = LabelNew::create(std::move(str));

		auto size = Engine::shared()->winSize();
		label->setPos2D(size.x / 2.0f - label->getContentSize().x / 2.0f, size.y - 150);
		g_GetCurrScene()->addNode(label);
		std::vector<Action * > seq;
		seq.push_back(new ActionInterval(duration));
		seq.push_back(new MoveBy(disappearDuration,vec3(0, 250, 0)));
		seq.push_back(new ActionRemoveMySelf());
		label->runAction(new ActionSequence(seq));
	}
}
