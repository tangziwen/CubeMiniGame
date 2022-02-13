#ifndef TZW_SPRITE_H
#define TZW_SPRITE_H
#include "../Base/Node.h"
#include "../Interface/Drawable2D.h"
#include "../Texture/TextureMgr.h"
#include "../Mesh/Mesh.h"
#include "../Math/vec2.h"
#include "../Math/vec4.h"
#include <string>
#include "../Event/Event.h"
namespace tzw {
class EventListener;
/**
  @class Sprite
  @brief 精灵——用以描述显示在屏幕空间上的2D图形元素
*/
class Sprite : public Drawable2D , public EventListener
{
public:
    Sprite();
    static Sprite * create(std::string texturePath);
    static Sprite * createWithColor(vec4 color,vec2 getContentSize);
    void initWithTexture(std::string texturePath);
    void initWithTexture(Texture * texture);
    void initWithColor(vec4 color, vec2 getContentSize);
    void submitDrawCmd(RenderFlag::RenderStage requirementType, RenderQueue * queues, int requirementArg) override;
    void setRenderRect(vec2 size, vec2 lb = vec2(0,0), vec2 rt= vec2(1,1), vec4 color = vec4(1,1,1,1) );
    void setContentSize(const vec2 &getContentSize) override;
    Texture *texture() const;
    void setTexture(Texture *texture);
    void setTexture(std::string texturePath);
    bool isUseTexture() const;
    void setIsUseTexture(bool isUseTexture);
    void setUpTechnique();
	void setUpTransFormation(TransformationInfo &info) override;
    void setColor(vec4 newColor) override;
	void setOverLayColor(vec4 newColor) override;
	bool onMouseRelease(int button,vec2 pos) override;
	bool onMousePress(int button,vec2 pos) override;
	bool onMouseMove(vec2 pos) override;
    void setTouchEnable(bool m_isEnable);
    std::string getSpriteManggledName();
    bool isInTheRect(vec2 touchPos);
    void setOnBtnClicked(const std::function<void (Sprite *)> &onBtnClicked);
protected:
    bool m_isUseTexture;
    Texture * m_texture;
    Mesh * m_mesh;
    bool m_isRenderRectDirty = false;
    vec2 m_lb = {0, 0};
    vec2 m_rt = {1, 1};
    bool isFirstTimeUpdateRenderRect = true;
    bool m_isTouchEnable = false;
    bool m_isTouched = false;
    std::function <void(Sprite *)> m_onBtnClicked;
};

} // namespace tzw

#endif // TZW_SPRITE_H
