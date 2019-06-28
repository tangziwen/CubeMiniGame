#ifndef TZW_PLAYER_H
#define TZW_PLAYER_H

#include "Game/FPSCamera.h"
#include "EngineSrc/3D/Model/Model.h"
#include "2D/GUISystem.h"
namespace tzw
{
	class GameItem;
	class CubePlayer : public Node, public EventListener, public IMGUIObject
	{
	public:
		enum class Mode
		{
			MODE_DEFORM_SPHERE,
			MODE_PLACE_CUBE,
			MODE_PLACE_SPHERE,
		};

		explicit CubePlayer(Node* mainRoot);
		FPSCamera* camera() const;
		void setCamera(FPSCamera* camera);
		vec3 getPos();
		void logicUpdate(float dt) override;
		bool checkIsNeedUpdateChunk();
		bool onKeyPress(int keyCode) override;
		bool onKeyRelease(int keyCode) override;
		bool onMousePress(int button, vec2 pos) override;
		void modeSwitch(Mode newMode);
		void drawIMGUI() override;
		void initSlots();
		void handleItemPrimaryUse(GameItem * item);
		void handleItemSecondaryUse(GameItem * item);
	private:
		std::vector<GameItem * > m_itemSlots;
		Mode m_currMode;
		FPSCamera* m_camera;
		Model* m_gunModel;
		int oldPosX;
		int oldPosZ;
		int m_currSelectItemIndex;
		bool m_enableGravity;
	};
} // namespace tzw

#endif // TZW_PLAYER_H
