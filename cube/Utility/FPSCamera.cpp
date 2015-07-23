#include "FPSCamera.h"
#include "Event/EventMgr.h"
namespace tzw {

FPSCamera::FPSCamera()
{
    init();
}
Tvector FPSCamera::velocity() const
{
    return m_velocity;
}

void FPSCamera::setVelocity(const Tvector &velocity)
{
    m_velocity = velocity;
}

void FPSCamera::handleKeyPress(KeyListener *self, int keycode)
{
    switch(keycode)
    {
    case Qt::Key_W:
        this->move_forward=true;
        break;
    case Qt::Key_S:
        this->move_backward=true;
        break;
    case Qt::Key_A:
        this->move_left=true;
        break;
    case Qt::Key_D:
        this->move_right=true;
        break;
    case Qt::Key_Q:
        this->move_up = true;
        break;
    case Qt::Key_E:
        this->move_down = true;
        break;
    }
}

void FPSCamera::handleKeyRelease(KeyListener *self, int keycode)
{
    switch(keycode)
    {
    case Qt::Key_W:
        this->move_forward=false;
        break;
    case Qt::Key_S:
        this->move_backward=false;
        break;
    case Qt::Key_A:
        this->move_left=false;
        break;
    case Qt::Key_D:
        this->move_right=false;
        break;
    case Qt::Key_Q:
        this->move_up = false;
        break;
    case Qt::Key_E:
        this->move_down = false;
        break;
    }
}

void FPSCamera::handleTouchPress(TouchListener *self, QVector2D pos)
{
    mousePressPosition = mouseLastPosition = pos;
}

void FPSCamera::handleTouchMove(TouchListener * self,QVector2D pos)
{
    QVector2D diff = pos - mouseLastPosition;
    if(diff.x()>2){
        yawBy(-1);
    }else if(diff.x()<-2)
    {
        yawBy(1);
    }
    if(diff.y ()>2)
    {
        pitchBy (-1);
    }else if(diff.y ()<-2)
    {
        pitchBy (1);
    }
    mouseLastPosition = pos;
}

void FPSCamera::handleTouchRelease(TouchListener *self, QVector2D pos)
{

}

void FPSCamera::handleOnRender(RenderListener *self, float dt)
{
    if(move_forward)
    {
        moveBy(0,0,-0.1);
    }
    if(move_backward){
        moveBy(0,0,0.1);
    }
    if(move_left)
    {
        moveBy(-0.1,0,0);
    }
    if(move_right)
    {
        moveBy(0.1,0,0);
    }
    if(move_up)
    {
        moveBy (0,0.1,0,false);
    }
    if(move_down)
    {
        moveBy (0,-0.1,0,false);
    }
}

void FPSCamera::init()
{
    auto touch = new TouchListener();
    touch->onTouchPress = std::bind(handleTouchPress,this,std::placeholders::_1,std::placeholders::_2);
    touch->onTouchMove = std::bind(handleTouchMove,this,std::placeholders::_1,std::placeholders::_2);
    touch->onTouchRelease = std::bind(handleTouchRelease,this,std::placeholders::_1,std::placeholders::_2);

    auto key = new KeyListener();
    key->onKeyPress = std::bind(handleKeyPress,this,std::placeholders::_1,std::placeholders::_2);
    key->onKeyRelease = std::bind(handleKeyRelease,this,std::placeholders::_1,std::placeholders::_2);

    auto renderListener = new RenderListener();
    renderListener->onRender = std::bind(handleOnRender,this,std::placeholders::_1,std::placeholders::_2);

    auto eventMgr = EventMgr::get ();
    eventMgr->addListener (touch);
    eventMgr->addListener (key);
    eventMgr->addListener (renderListener);



    this->move_forward=false;
    this->move_backward=false;
    this->move_left=false;
    this->move_right=false;
    this->move_up = false;
    this->move_down = false;
}


} // namespace tzw

