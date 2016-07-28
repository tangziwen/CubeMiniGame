#ifndef TZW_BOXCONTAINER_H
#define TZW_BOXCONTAINER_H
#include "../2D/GUIFrame.h"
#include "../Interface/Drawable2D.h"
#include <vector>
namespace tzw {

class BoxContainer : public Drawable2D
{
public:
    BoxContainer();
    static BoxContainer* create(vec2 size, vec2 boxSize);
    void insert(Drawable2D * theElement);
    void format();
    vec2 boxSize() const;
    void setBoxSize(const vec2 &boxSize);
    virtual vec2 getContentSize();
    virtual void setContentSize(vec2 Size);
private:
    GUIFrame * m_frameBG;
    std::vector<Drawable2D *> m_elementList;
    vec2 m_boxSize;
};

} // namespace tzw

#endif // TZW_BOXCONTAINER_H
