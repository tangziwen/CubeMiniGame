#include "KeyListener.h"

KeyListener::KeyListener()
    :onKeyPress(nullptr),onKeyRelease(nullptr)
{
}

KeyListener::~KeyListener()
{

}

void KeyListener::handleKeyPress(int keycode)
{

}

void KeyListener::handleKeyRelease(int keycode)
{

}

BaseListener::Type KeyListener::getListenerType()
{
    return Type::KeyListener;
}

