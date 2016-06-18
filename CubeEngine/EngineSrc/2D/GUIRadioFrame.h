#ifndef TZW_GUIRADIOFRAME_H
#define TZW_GUIRADIOFRAME_H

#include "GUITitledFrame.h"
#include <vector>
#include "Button.h"
#include <map>
namespace tzw {

class GUIRadioFrame :public GUITitledFrame
{
public:
    enum class ExpandType
    {
        Vertical,
        Horizontal,
    };

    GUIRadioFrame();
    static GUIRadioFrame * create(std::string titleText, vec4 color,vec2 size);
    static GUIRadioFrame * create(std::string titleText,vec2 size);
    static GUIRadioFrame * create(std::string titleText);
    void addRadioButton(std::string buttonName, std::function<void (Button *)> btnCallback = nullptr);
    ExpandType getExpandType() const;
    void setExpandType(const ExpandType &expandType);
    void flush();
    float getStride() const;
    void setStride(float stride);

    float getBtnsStride() const;
    void setBtnsStride(float btnsStride);

    LabelNew *getTipsLabel() const;

    LabelNew *getDetailLabel() const;

    vec2 getMarginHorizontal() const;
    void setMarginHorizontal(const vec2 &marginHorizontal);

private:

    void onButtonClicked(Button * btn);
    void flushVertical();
    void flushHorizontal();
    void focusAndTintColor(Button *btn);
    void initTipsLabel();
    void initDetailLabel();
private:
    float m_stride;
    float m_btnsStride;
    vec2 m_marginHorizontal;
    Button * m_focusBtn;
    LabelNew * m_tipsLabel;
    LabelNew * m_detailLabel;
    ExpandType m_expandType;
    std::vector<Button *>m_btnList;
    std::map<Button * ,std::function<void (Button*)>> m_buttonCallbackMap;
};

} // namespace tzw

#endif // TZW_GUIRADIOFRAME_H
