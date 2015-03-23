#include "backend.h"
#include <QApplication>
#include <QLabel>

#ifndef QT_NO_OPENGL
#include "mainwidget.h"
#endif

int backend::start(RenderDelegate *delegate, int argc, char *argv[])
{

    QApplication app(argc, argv);
    app.setApplicationName("cube ");
    app.setApplicationVersion("0.1");
#ifndef QT_NO_OPENGL
    MainWidget widget(0,delegate);
    widget.resize (1024,768);
    widget.show();
#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
    return app.exec();
}

