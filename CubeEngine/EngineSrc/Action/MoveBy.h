#ifndef TZW_MOVEBY_H
#define TZW_MOVEBY_H
#include "ActionInterval.h"
#include "../Math/vec3.h"
namespace tzw
{
	class MoveBy :public ActionInterval
	{
	public:
		MoveBy(float duration, vec3 speed);
		void step(Node *node, float dt) override;
		virtual void final(Node * node);
		virtual ~MoveBy();
	private:
		vec3 m_speed;
	};
}

#endif
