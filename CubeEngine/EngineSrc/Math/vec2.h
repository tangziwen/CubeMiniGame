#ifndef TZW_VEC2_H
#define TZW_VEC2_H


namespace tzw {

class vec2
{
public:
    vec2();
    vec2(float the_x,float the_y);
    void set(float the_x,float the_y);
    vec2 operator + (const vec2 & other) const;
    vec2 operator - (const vec2 & other) const;
    vec2 operator / (const float & other) const;
    float x,y;
    float getX() const;
    void setX(float value);
    float getY() const;
    void setY(float value);
};

} // namespace tzw

#endif // TZW_VEC2_H
