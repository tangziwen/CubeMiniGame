#ifndef TZW_GUIATTRIBUTELABEL_H
#define TZW_GUIATTRIBUTELABEL_H

#include "../Interface/Drawable2D.h"
#include "GUIFrame.h"
#include "LabelNew.h"
namespace tzw {

class GUIAttributeLabel : public Drawable2D
{
public:
    enum class AlignMode
    {
        Centre,
        Left,
    };

    GUIAttributeLabel(std::string theDesc,float theValue);
    GUIAttributeLabel();
    bool isNeedShowBG() const;
    void setIsNeedShowBG(bool isNeedShowBG);
    virtual vec2 getContentSize();
    static GUIAttributeLabel * createWithFixedSize(std::string theDesc, float theValue, vec2 Size, AlignMode alignMode = AlignMode::Left);
    static GUIAttributeLabel * create(std::string theDesc,float theValue);
    std::string getDesc() const;
    void setDesc(const std::string &desc);
    float getValueF() const;
    void setValueF(float valueF);

    vec2 getMargin() const;
    void setMargin(const vec2 &margin);

protected:
    void constructUI();
    GUIFrame * m_frame;
    LabelNew * m_label;
    std::string m_desc;
    vec2 m_margin;
    float m_valueF;
    bool m_isNeedShowBG;
    bool m_isFixedSize;
};

} // namespace tzw

#endif // TZW_GUIATTRIBUTELABEL_H
