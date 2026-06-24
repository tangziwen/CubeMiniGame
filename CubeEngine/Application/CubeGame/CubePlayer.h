#ifndef TZW_PLAYER_H
#define TZW_PLAYER_H

#include "Game/FPSCamera.h"
#include "EngineSrc/3D/Model/Model.h"
#include "2D/IMGUISystem.h"
#include "Game/OrbitCamera.h"
#include "Island.h"
#include "PaintGun.h"
#include "PreviewItem.h"

namespace tzw
{
	struct Attachment;
	struct GameItem;
	class Camera;
	class EditorCamera;
	class GamePart;
	class PartSurface;
	class Audio;
	class AudioEvent;
	class FPGun;
	enum class PlayerMode;
	class CubePlayer : public EventListener, public IMGUIObject
	{
	public:
		enum class Mode
		{
			MODE_DEFORM_SPHERE,
			MODE_PLACE_CUBE,
			MODE_PLACE_SPHERE,
		};

		enum class InSeatViewMode
		{
			FPV,
			ORBIT,
		};
		explicit CubePlayer(Node* mainRoot);
		FPSCamera* camera() const;
		EditorCamera* editorCamera() const;
		void setCamera(FPSCamera* camera);
		vec3 getPos();
		void setPos(vec3 newPos);
		void onFrameUpdate(float dt) override;
		bool onKeyPress(int keyCode) override;
		bool onKeyRelease(int keyCode) override;
		bool onMousePress(int button, vec2 pos) override;
		void modeSwitch(Mode newMode);
		void initSlots();
		vec3 getForward() const;
		void sitDownToGamePart(GamePart * part);
		void standUpFromGamePart(GamePart * part);
		void removePartByAttach(Attachment* attach);
		void removePart(GamePart* part);
		void setCurrPointPart(GamePart * part);
		void removeAllBlocks();
		void updateCrossHairTipsInfo();
		void openCurrentPartInspectMenu();
		GamePart * getCurrPointPart();
		PaintGun * getPaintGun();
		void paint();
		void setCurrSelected(std::string itemName);
		void setPreviewAngle(float angle);
		float getPreviewAngle() const;
		PreviewItem * getPreviewItem();
		void pressButton(GamePart *  buttonPart);
		void releaseButton(GamePart * buttonPart);
		void releaseSwitch(GamePart * switchPart);
		InSeatViewMode getSeatViewMode() const;
		void setSeatViewMode(const InSeatViewMode seatViewMode);
		void setIsOpenJetPack(bool isOpen);

		bool onScroll(vec2 offset) override;
		GameItem * getCurSelectedItem();
		Quaternion getRotateQ() const;

		void setRotateQ(const Quaternion &rotateQ);
		void setRotateQ(const vec4 &rotateQInV4);
		FPGun * getGun();
	private:
		Camera* activeViewCamera() const;
		void onPlayerModeChanged(PlayerMode mode);
		void onHitGround();
		void handleSitDown();
		PreviewItem * m_previewItem;
		std::vector<GameItem * > m_itemSlots;
		Mode m_currMode;
		FPSCamera* m_camera;
		EditorCamera* m_editorCamera;
		OrbitCamera * m_orbitcamera;
		FPSCamera * m_fpvCamera;
		FPGun* m_gunModel;
		int m_currSelectItemIndex;
		bool m_enableGravity;
		float m_seatAngle = 0.0;
		GameItem* m_currSelectedItem;
		PaintGun * m_paintGun;
		GamePart * m_currPointPart;
		InSeatViewMode m_seatViewMode;
		Audio * m_footstep;
		Audio * m_hitGroundSound;
		vec3 ADSPos;
		vec3 hipPos;
		bool isAdsMode = false;
		std::unordered_map<std::string, FPGun *> m_gunDict;
	};
} // namespace tzw

#endif // TZW_PLAYER_H
