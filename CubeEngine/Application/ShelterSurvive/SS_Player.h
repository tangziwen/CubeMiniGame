#ifndef SS_PLAYER_H
#define SS_PLAYER_H
#include "Game/FPSCamera.h"
#include "EngineSrc/3D/Model/Model.h"

namespace tzw
{
	class SS_Player : public Node, public EventListener
	{

	public:
		SS_Player(Node * mainRoot);
		FPSCamera *camera() const;
		void setCamera(FPSCamera *camera);
		vec3 getPos();
		virtual void logicUpdate(float dt);
		bool checkIsNeedUpdateChunk();
		virtual bool onKeyPress(int keyCode);
		virtual bool onKeyRelease(int keyCode);
	private:
		FPSCamera * m_camera;
		Model * m_gunModel;
		int oldPosX;
		int oldPosZ;
	};

}
#endif
