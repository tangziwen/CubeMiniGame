#ifndef TZW_EVENT_H
#define TZW_EVENT_H
#include "../Math/vec2.h"
#include <string>

/* The unknown key */
#define TZW_KEY_UNKNOWN            -1

/* Printable keys */
#define TZW_KEY_SPACE              32
#define TZW_KEY_APOSTROPHE         39  /* ' */
#define TZW_KEY_COMMA              44  /* , */
#define TZW_KEY_MINUS              45  /* - */
#define TZW_KEY_PERIOD             46  /* . */
#define TZW_KEY_SLASH              47  /* / */
#define TZW_KEY_0                  48
#define TZW_KEY_1                  49
#define TZW_KEY_2                  50
#define TZW_KEY_3                  51
#define TZW_KEY_4                  52
#define TZW_KEY_5                  53
#define TZW_KEY_6                  54
#define TZW_KEY_7                  55
#define TZW_KEY_8                  56
#define TZW_KEY_9                  57
#define TZW_KEY_SEMICOLON          59  /* ; */
#define TZW_KEY_EQUAL              61  /* = */
#define TZW_KEY_A                  65
#define TZW_KEY_B                  66
#define TZW_KEY_C                  67
#define TZW_KEY_D                  68
#define TZW_KEY_E                  69
#define TZW_KEY_F                  70
#define TZW_KEY_G                  71
#define TZW_KEY_H                  72
#define TZW_KEY_I                  73
#define TZW_KEY_J                  74
#define TZW_KEY_K                  75
#define TZW_KEY_L                  76
#define TZW_KEY_M                  77
#define TZW_KEY_N                  78
#define TZW_KEY_O                  79
#define TZW_KEY_P                  80
#define TZW_KEY_Q                  81
#define TZW_KEY_R                  82
#define TZW_KEY_S                  83
#define TZW_KEY_T                  84
#define TZW_KEY_U                  85
#define TZW_KEY_V                  86
#define TZW_KEY_W                  87
#define TZW_KEY_X                  88
#define TZW_KEY_Y                  89
#define TZW_KEY_Z                  90
#define TZW_KEY_LEFT_BRACKET       91  /* [ */
#define TZW_KEY_BACKSLASH          92  /* \ */
#define TZW_KEY_RIGHT_BRACKET      93  /* ] */
#define TZW_KEY_GRAVE_ACCENT       96  /* ` */
#define TZW_KEY_WORLD_1            161 /* non-US #1 */
#define TZW_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define TZW_KEY_ESCAPE             256
#define TZW_KEY_ENTER              257
#define TZW_KEY_TAB                258
#define TZW_KEY_BACKSPACE          259
#define TZW_KEY_INSERT             260
#define TZW_KEY_DELETE             261
#define TZW_KEY_RIGHT              262
#define TZW_KEY_LEFT               263
#define TZW_KEY_DOWN               264
#define TZW_KEY_UP                 265
#define TZW_KEY_PAGE_UP            266
#define TZW_KEY_PAGE_DOWN          267
#define TZW_KEY_HOME               268
#define TZW_KEY_END                269
#define TZW_KEY_CAPS_LOCK          280
#define TZW_KEY_SCROLL_LOCK        281
#define TZW_KEY_NUM_LOCK           282
#define TZW_KEY_PRINT_SCREEN       283
#define TZW_KEY_PAUSE              284
#define TZW_KEY_F1                 290
#define TZW_KEY_F2                 291
#define TZW_KEY_F3                 292
#define TZW_KEY_F4                 293
#define TZW_KEY_F5                 294
#define TZW_KEY_F6                 295
#define TZW_KEY_F7                 296
#define TZW_KEY_F8                 297
#define TZW_KEY_F9                 298
#define TZW_KEY_F10                299
#define TZW_KEY_F11                300
#define TZW_KEY_F12                301
#define TZW_KEY_F13                302
#define TZW_KEY_F14                303
#define TZW_KEY_F15                304
#define TZW_KEY_F16                305
#define TZW_KEY_F17                306
#define TZW_KEY_F18                307
#define TZW_KEY_F19                308
#define TZW_KEY_F20                309
#define TZW_KEY_F21                310
#define TZW_KEY_F22                311
#define TZW_KEY_F23                312
#define TZW_KEY_F24                313
#define TZW_KEY_F25                314
#define TZW_KEY_KP_0               320
#define TZW_KEY_KP_1               321
#define TZW_KEY_KP_2               322
#define TZW_KEY_KP_3               323
#define TZW_KEY_KP_4               324
#define TZW_KEY_KP_5               325
#define TZW_KEY_KP_6               326
#define TZW_KEY_KP_7               327
#define TZW_KEY_KP_8               328
#define TZW_KEY_KP_9               329
#define TZW_KEY_KP_DECIMAL         330
#define TZW_KEY_KP_DIVIDE          331
#define TZW_KEY_KP_MULTIPLY        332
#define TZW_KEY_KP_SUBTRACT        333
#define TZW_KEY_KP_ADD             334
#define TZW_KEY_KP_ENTER           335
#define TZW_KEY_KP_EQUAL           336
#define TZW_KEY_LEFT_SHIFT         340
#define TZW_KEY_LEFT_CONTROL       341
#define TZW_KEY_LEFT_ALT           342
#define TZW_KEY_LEFT_SUPER         343
#define TZW_KEY_RIGHT_SHIFT        344
#define TZW_KEY_RIGHT_CONTROL      345
#define TZW_KEY_RIGHT_ALT          346
#define TZW_KEY_RIGHT_SUPER        347
#define TZW_KEY_MENU               348
#define TZW_KEY_LAST               TZW_KEY_MENU

namespace tzw {
class Node;
class EventMgr;

class EventListener
{
public:
    virtual bool onKeyPress(int keyCode);
    virtual ~EventListener();
    virtual bool onKeyRelease(int keyCode);
    virtual bool onCharInput(unsigned int theChar);
    virtual bool onMouseRelease(int button,vec2 pos);
    virtual bool onMousePress(int button,vec2 pos);
    virtual bool onMouseMove(vec2 pos);
	virtual bool onScroll(vec2 offset);
    virtual void onFrameUpdate(float delta);
    EventListener();
    unsigned int getFixedPiority() const;
    void setFixedPiority(unsigned int getFixedPiority);
    bool isSwallow() const;
    void setIsSwallow(bool isSwallow);
    EventMgr *getMgr() const;
    void setMgr(EventMgr *parent);
    Node *attachNode() const;
    void setAttachNode(Node *attachNode);
protected:
    Node * m_attachNode;
    EventMgr * m_mgr;
    bool m_isSwallow;
    unsigned int m_fixedPiority;
};

} // namespace tzw

#endif // TZW_EVENT_H
