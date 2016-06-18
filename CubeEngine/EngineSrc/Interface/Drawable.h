#ifndef TZW_DRAWABLE_H
#define TZW_DRAWABLE_H

#include "../Base/Node.h"
#include "../Base/Camera.h"
#include "../Technique/Technique.h"
namespace tzw {
/**
 * @brief The Drawable class Drawable类是一个接口，主要用以表征一个类是可以被实际绘制到屏幕上的，
 * 所有能触发实际绘制的类，都必须实现该接口
 */
class Drawable : public Node
{
public:
    Drawable();
    Camera *camera() const;
    void setCamera(Camera *camera);
    Technique *technique() const;
    void setTechnique(Technique *technique);
    virtual Node::NodeType getNodeType();
    vec4 getUniformColor() const;
    virtual void setUniformColor(const vec4 &uniformColor);
    virtual void setUniformColor(const vec3 &color);
    virtual void setAlpha(float alpha);
    virtual void reCache();
protected:
    vec4 m_uniformColor;
    Camera * m_camera;
    Technique * m_technique;
};

} // namespace tzw

#endif // TZW_DRAWABLE_H
