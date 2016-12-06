#ifndef SS_WORLD_H
#define SS_WORLD_H
#include "Engine/EngineDef.h"
namespace tzw
{
	class SS_World
	{
	public:
		void createWorld();
		void createTest();
	private:
		TZW_SINGLETON_DECL(SS_World)
	};

}

#endif // !SS_WORLD_H
