#include "Rect.h"


Rect::Rect(void)
{
        setRect(0.0f, 0.0f, 0.0f, 0.0f);
}

Rect::Rect(QVector2D a, QVector2D b)
{
        setRect(a.x(), a.y(), b.x () - a.x (), b.y () - a.y ());
}

Rect::Rect(float x, float y, float width, float height)
{
        setRect(x, y, width, height);
}

Rect::Rect(const Rect& other)
{
        setRect(other.origin.x(), other.origin.y(), other.size.x(), other.size.y());
}

Rect& Rect::operator= (const Rect& other)
{
        setRect(other.origin.x(), other.origin.y(), other.size.x(), other.size.y());
        return *this;
}

void Rect::setRect(float x, float y, float width, float height)
{
        // CGRect can support width<0 or height<0
        // CCASSERT(width >= 0.0f && height >= 0.0f, "width and height of Rect must not less than 0.");

        origin.setX (x);
        origin.setY(y);

        size.setX (width);
        size.setY (height);
}

bool Rect::equals(const Rect& rect) const
{
        return true;
}

float Rect::getMaxX() const
{
        return origin.x() + size.x();
}

float Rect::getMidX() const
{
        return origin.x() + size.x() / 2.0f;
}

float Rect::getMinX() const
{
        return origin.x();
}

float Rect::getMaxY() const
{
        return origin.y() + size.y();
}

float Rect::getMidY() const
{
        return origin.y() + size.y() / 2.0f;
}

float Rect::getMinY() const
{
        return origin.y();
}

bool Rect::containsPoint(const QVector2D& point) const
{
        bool bRet = false;

        if (point.x() >= getMinX() && point.x() <= getMaxX()
                        && point.y() >= getMinY() && point.y() <= getMaxY())
        {
                bRet = true;
        }

        return bRet;
}

bool Rect::intersectsRect(const Rect& rect) const
{
        return !(     getMaxX() < rect.getMinX() ||
                      rect.getMaxX() <      getMinX() ||
                      getMaxY() < rect.getMinY() ||
                      rect.getMaxY() <      getMinY());
}

bool Rect::intersectsCircle(const QVector2D &center, float radius) const
{
        return true;
}

void Rect::merge(const Rect& rect)
{
        float top1    = getMaxY();
        float left1   = getMinX();
        float right1  = getMaxX();
        float bottom1 = getMinY();

        float top2    = rect.getMaxY();
        float left2   = rect.getMinX();
        float right2  = rect.getMaxX();
        float bottom2 = rect.getMinY();
        origin.setX (std::min(left1, left2));
        origin.setY (std::min(bottom1, bottom2));
        size.setX (std::max(right1, right2) - origin.x());
        size.setY (std::max(top1, top2) - origin.y());
}

Rect Rect::unionWithRect(const Rect & rect) const
{
        float thisLeftX = origin.x();
        float thisRightX = origin.x() + size.x();
        float thisTopY = origin.y() + size.y();
        float thisBottomY = origin.y();

        if (thisRightX < thisLeftX)
        {
                std::swap(thisRightX, thisLeftX);   // This rect has negative width
        }

        if (thisTopY < thisBottomY)
        {
                std::swap(thisTopY, thisBottomY);   // This rect has negative height
        }

        float otherLeftX = rect.origin.x();
        float otherRightX = rect.origin.x() + rect.size.x();
        float otherTopY = rect.origin.y() + rect.size.y();
        float otherBottomY = rect.origin.y();

        if (otherRightX < otherLeftX)
        {
                std::swap(otherRightX, otherLeftX);   // Other rect has negative width
        }

        if (otherTopY < otherBottomY)
        {
                std::swap(otherTopY, otherBottomY);   // Other rect has negative height
        }

        float combinedLeftX = std::min(thisLeftX, otherLeftX);
        float combinedRightX = std::max(thisRightX, otherRightX);
        float combinedTopY = std::max(thisTopY, otherTopY);
        float combinedBottomY = std::min(thisBottomY, otherBottomY);

        return Rect(combinedLeftX, combinedBottomY, combinedRightX - combinedLeftX, combinedTopY - combinedBottomY);
}

const Rect Rect::ZERO = Rect(0, 0, 0, 0);
