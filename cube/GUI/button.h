#ifndef BUTTON_H
#define BUTTON_H

#include "texture/texturepool.h"
#include "sprite.h"

class Button : public Sprite
{
public:
    Button();
    void init(const char *texture, const char *texture_pressed = nullptr);
    virtual bool checkTouchPress(QVector2D pos);

    virtual void handleTouchPress(QVector2D pos);
    virtual void handleTouchRelease(QVector2D pos);
private:
    Texture * textureNormal;
    Texture * texturePressed;
};

#endif // BUTTON_H
