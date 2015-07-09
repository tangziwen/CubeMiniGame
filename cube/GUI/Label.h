#ifndef LABEL_H
#define LABEL_H
#include <string>
#include "GUI/sprite.h"
#include <vector>
#include "font/BMFontConfiguration.h"

class Label : public Node
{
public:
    Label();
    ~Label();
    void setText(std::string string);
    int fontSize() const;
    void setFontSize(int fontSize);
private:
    void getTextureOffset(QVector2D & TL, QVector2D & BR, const _BMFontDef &def);
    float scaleByFontSize(float value);
private:
    int m_fontSize;
    std::string m_string;
    std::vector<Sprite * >m_glyphs;
    Texture * m_texture;
    BMFontConfiguration * _config;
};

#endif // LABEL_H
