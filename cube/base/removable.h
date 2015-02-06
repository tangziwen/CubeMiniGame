#ifndef REMOVABLE_H
#define REMOVABLE_H


class Removable
{
public:
    Removable();
    bool isRemoved() const;
    void setIsRemoved(bool isRemoved);
private:
    bool m_isRemoved;
};

#endif // REMOVABLE_H
