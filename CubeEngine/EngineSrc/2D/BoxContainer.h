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
    void insert(Node * theElement);
    void format();
    vec2 boxSize() const;
    void setBoxSize(const vec2 &boxSize);
    vec2 getContentSize() const override;
    void setContentSize(const vec2 &contentSize) override;
private:
    GUIFrame * m_frameBG;
    std::vector<Node *> m_elementList;
    vec2 m_boxSize;
};

} // namespace tzw

#endif // TZW_BOXCONTAINER_H
