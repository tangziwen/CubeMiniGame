#include "Label.h"


Label::Label()
    :m_fontSize(32)
{
    _config= BMFontConfiguration::create("./res/texture/font/futura-48.fnt");
    m_texture = TexturePool::getInstance ()->createOrGetTexture (_config->getAtlasName ().c_str ());
}

Label::~Label()
{

}

void Label::setText(std::string string)
{
    m_string = string;
    int pos_x = 0;
    removeAllChildren ();
    for(int i =0;i<m_string.size ();i++)
    {
        char c = m_string[i];
        QVector2D TL,BR;
        auto def = _config->getFontDef (c);
        getTextureOffset (TL,BR,def);
        auto sprite = new Sprite;
        sprite->setTexture (m_texture);
        sprite->setPos (QVector3D(pos_x,0,0));

        pos_x += scaleByFontSize(def.xAdvance);
        float w = scaleByFontSize(def.rect.size.x ());
        float h = scaleByFontSize(def.rect.size.y ());
        sprite->setSize (w,h);
        sprite->setCamera (camera());
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

void Label::getTextureOffset(QVector2D &TL, QVector2D &BR, const _BMFontDef & def)
{
    TL = def.rect.origin;
    TL.setX (TL.x ()/m_texture->width ());
    TL.setY (TL.y ()/m_texture->height ());
    BR = def.rect.origin + def.rect.size;
    BR.setX (BR.x ()/m_texture->width ());
    BR.setY (BR.y ()/m_texture->height ());
}

float Label::scaleByFontSize(float value)
{
    return 1.0*value/_config->fontSize () * m_fontSize;
}


