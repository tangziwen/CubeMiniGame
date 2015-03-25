/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwidget.h"

#include <QMouseEvent>

#include <math.h>
#include "utility.h"
#include "texture/texturepool.h"

MainWidget::MainWidget(QWidget *parent,RenderDelegate *delegate) :
    QGLWidget(parent)
{
    this->delegate = delegate;
     isKeyPressed = false;
     isKeyRelease = false;
     isMouseMove = false;
     isMousePress = false;
     isMouseRelease = false;
}

MainWidget::~MainWidget()
{
}


void MainWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(delegate)
    {
        delegate->onTouchMove(e->localPos().x(),e->localPos().y());
    }
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
    if(delegate)
    {
        delegate->onTouchBegin(e->localPos().x(),e->localPos().y());
    }

}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(delegate)
    {
        delegate->onTouchEnd(e->localPos().x(),e->localPos().y());
    }
}

void MainWidget::timerEvent(QTimerEvent *)
{
    updateGL();
}

void MainWidget::keyPressEvent(QKeyEvent *event)
{
    if(delegate)
    {
        delegate->onKeyPress(event->key());
    }
}

void MainWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(delegate)
    {
        delegate->onKeyRelease(event->key());
    }
}


void MainWidget::initializeGL()
{
    initializeGLFunctions();
    glClearColor(0,0,0,1);
    glEnable(GL_TEXTURE_2D);
    initShaders();
    initTextures();
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    //Enable back face culling
    glEnable(GL_CULL_FACE);
    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
    if(delegate)
    {
        delegate->onInit();
    }
}

void MainWidget::initShaders()
{


}
void MainWidget::initTextures()
{
}
void MainWidget::resizeGL(int w, int h)
{
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    if(delegate)
    {
        delegate->onResize(w,h);
    }
}
void MainWidget::paintGL()
{

    if(delegate)
    {
        delegate->onRender();
    }
    Scene::getCurrentScene()->render();
}
