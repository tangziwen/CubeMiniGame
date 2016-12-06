#include "SS_Player.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "EngineSrc/3D/Model/Model.h"
#include "EngineSrc/Event/EventMgr.h"
namespace tzw {

	SS_Player::SS_Player(Node *mainRoot)
	{
		FPSCamera * camera = FPSCamera::create(g_GetCurrScene()->defaultCamera());
		camera->setPos(vec3(0,10,0));
		mainRoot->addChild(camera);
		g_GetCurrScene()->setDefaultCamera(camera);
		m_camera = camera;
		m_camera->setIsEnableGravity(true);

		//the gun
		m_gunModel = Model::create("./Res/diediedie.tzw");
		m_gunModel->setScale(vec3(0.005, 0.005, 0.005));
		m_gunModel->setRotateE(vec3(0, -90, 0));
		m_gunModel->setPos(vec3(0.08,-0.20,-0.25));
		m_gunModel->setIsAccpectOCTtree(false);
		m_camera->addChild(m_gunModel);
		EventMgr::shared()->addFixedPiorityListener(this);
	}

	FPSCamera *SS_Player::camera() const
	{
		return m_camera;
	}

	void SS_Player::setCamera(FPSCamera *camera)
	{
		m_camera = camera;
	}

	vec3 SS_Player::getPos()
	{
		return m_camera->getPos();
	}

	void SS_Player::logicUpdate(float dt)
	{
		static float  theTime = 0.0f;
		vec3 oldPos = m_gunModel->getPos();
		float offset = 0.002;
		float freq = 1;
		if(m_camera->getIsMoving())
		{
			offset = 0.006;
			freq = 6;
		}
		theTime += freq * dt;
		m_gunModel->setPos(vec3(oldPos.x, -0.2 + sinf(theTime) * offset, oldPos.z));
	}

	bool SS_Player::onKeyPress(int keyCode)
	{
		return false;
	}

	bool SS_Player::onKeyRelease(int keyCode)
	{
		switch(keyCode)
		{
		default:
			break;
		}
		return false;
	}
} // namespace tzw
