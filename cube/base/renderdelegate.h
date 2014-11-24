#ifndef RENDERDELEGATE_H
#define RENDERDELEGATE_H


class RenderDelegate
{
public:
    virtual void onInit() = 0 ;
    virtual void onRender() = 0 ;
    virtual void onResize(int w, int h) = 0;

    virtual void onKeyPress(int key_code);
    virtual void onKeyRelease(int key_code);
    virtual void onTouchBegin(int x,int y);
    virtual void onTouchMove(int x , int y);
    virtual void onTouchEnd(int x , int y);
};

#endif // RENDERDELEGATE_H
