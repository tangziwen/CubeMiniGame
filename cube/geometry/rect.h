#ifndef RECT_H
#define RECT_H
#include <QVector2D>

class Rect
{
public:
    Rect(QVector2D bl,QVector2D tr);
    bool isInRect(QVector2D pos);
private:
    QVector2D m_bl,m_tr;
};

#endif // RECT_H
