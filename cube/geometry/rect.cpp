#include "rect.h"

Rect::Rect(QVector2D bl,QVector2D tr):
    m_bl(bl),m_tr(tr)
{

}

bool Rect::isInRect(QVector2D pos)
{
    if(pos.x () > m_bl.x () && pos.x () < m_tr.x ()
            && pos.y () > m_bl.y () && pos.y() < m_tr.y ())
    {
        return true;
    }else
    {
        return false;
    }
}

