#ifndef TZW_TINT_TO_H
#define TZW_TINT_TO_H
#include "ActionInterval.h"
#include "../Math/vec3.h"
#include "../Math/vec4.h"
namespace tzw
{
	class TintTo :public ActionInterval
	{
	public:
		TintTo(float duration, vec4 fromColor, vec4 toColor);
		virtual void step(Node *node, float dt);
		virtual void final(Node * node);
		virtual ~TintTo();
	private:
		vec4 m_fromColor;
		vec4 m_toColor;
	};
}

#endif // !TZW_TINT_TO_H
