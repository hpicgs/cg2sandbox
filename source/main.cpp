
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
        Viewer viewer;

        QSurfaceFormat format;
        format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);

        Canvas canvas(format);
        canvas.setContinuousRepaint(true, 0);
        canvas.setSwapInterval(Canvas::VerticalSyncronization);

        QObject::connect(&canvas, &Canvas::fpsUpdate, &viewer, &Viewer::fpsChanged);
        QObject::connect(&viewer, &Viewer::toggleSwapInterval, &canvas, &Canvas::toggleSwapInterval);

        Painter painter;
        canvas.assignPainter(&painter);

        QWidget * widget = QWidget::createWindowContainer(&canvas);
        widget->setMinimumSize(1, 1);
        widget->setAutoFillBackground(false); // Important for overdraw, not occluding the scene.
        widget->setFocusPolicy(Qt::TabFocus);

        viewer.setCentralWidget(widget);
        viewer.show();

        result = app.exec();
    }
    return result;
}