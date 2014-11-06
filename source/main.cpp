
#include <QSurfaceFormat>
#include <QWidget>

#include "Application.h"
#include "Viewer.h"

int main(int argc, char * argv[])
{
    int result = -1;

    Application app(argc, argv);

    QSurfaceFormat format;
#ifdef NO_OPENGL_320
    format.setVersion(3, 1);
#else
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
#endif

    format.setDepthBufferSize(24);

    QScopedPointer<Viewer> viewer(new Viewer(format));
    result = app.exec();

    return result;
}
