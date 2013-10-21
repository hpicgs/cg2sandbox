
#include <QSurfaceFormat>
#include <QWidget>

#include "Application.h"

#include "Viewer.h"
#include "Canvas.h"

#include "Painter.h"


int main(int argc, char* argv[])
{
    int result = -1;

    Application app(argc, argv);

    {
        QScopedPointer<Viewer> viewer(new Viewer());

        QSurfaceFormat format;
#ifdef NO_OPENGL_320
        format.setVersion(3, 0);
#else
        format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);
#endif
        Canvas * canvas = new Canvas(format);
        canvas->setContinuousRepaint(true, 0);
        canvas->setSwapInterval(Canvas::VerticalSyncronization);

        QObject::connect(canvas, &Canvas::fpsUpdate, viewer.data(), &Viewer::fpsChanged);
        QObject::connect(canvas, &Canvas::objUpdate, viewer.data(), &Viewer::objChanged);
        QObject::connect(canvas, &Canvas::timeUpdate, viewer.data(), &Viewer::timeChanged);
        QObject::connect(canvas, &Canvas::mouseUpdate, viewer.data(), &Viewer::mouseChanged);
        QObject::connect(viewer.data(), &Viewer::toggleSwapInterval, canvas, &Canvas::toggleSwapInterval);


        Painter painter;
        canvas->assignPainter(&painter);

        QWidget * widget = QWidget::createWindowContainer(canvas);
        widget->setMinimumSize(1, 1);
        widget->setAutoFillBackground(false); // Important for overdraw, not occluding the scene.
        widget->setFocusPolicy(Qt::TabFocus);

        viewer->setCentralWidget(widget);
        viewer->show();

        result = app.exec();
        canvas->assignPainter(nullptr);
    }
    return result;
}