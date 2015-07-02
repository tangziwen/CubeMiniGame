#include "Label.h"

Label::Label()
    :m_fontSize(16)
{
    m_texture = TexturePool::getInstance ()->createOrGetTexture ("./res/texture/font/core.png");
}

Label::~Label()
{

}

void Label::setText(std::string string)
{
    m_string = string;
    for(int i =0;i<m_string.size ();i++)
    {
        auto sprite = new Sprite;
        sprite->setTexture (m_texture);
        sprite->setPos (QVector3D(i*m_fontSize,0,0));
        sprite->setSize (m_fontSize,m_fontSize);
        sprite->setCamera (camera());

        QVector2D TL,BR;
        char c = m_string[i];
        getTextureOffset (TL,BR,c);
        sprite->setRect (TL,BR);
        addChild(sprite);
    }
}
int Label::fontSize() const
{
    return m_fontSize;
}

void Label::setFontSize(int fontSize)
{
    m_fontSize = fontSize;
}

void Label::getTextureOffset(QVector2D &TL, QVector2D &BR,char c)
{
    c -= 32;
    int x_offset = c%8;
    int y_offset = c/8;
    TL.setX(1.0*x_offset/8);
    TL.setY(1.0*y_offset/8);
    BR.setX(TL.x () + 1.0/8);
    BR.setY(TL.y () + 1.0/8);
}


