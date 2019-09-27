#include "ActionRemoveMySelf.h"
#include "Base/Node.h"
namespace tzw {

ActionRemoveMySelf::ActionRemoveMySelf()
	:m_isOk(false)
{

}

void ActionRemoveMySelf::update(Node* node, float dt)
{
	node->removeFromParent();
	delete node;
	m_isOk = true;
}

bool ActionRemoveMySelf::isDone()
{
    return m_isOk;
}

} // namespace tzw
