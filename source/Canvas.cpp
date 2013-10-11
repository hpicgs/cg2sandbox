
#include <cassert>

#include <QDebug>
#include <QApplication>
#include <QBasicTimer>
#include <QResizeEvent>

#include "AbstractPainter.h"
#include "FileAssociatedShader.h"
#include "Camera.h"
#include "Navigation.h"
#include "Timer.h"
#include "Canvas.h"


Canvas::Canvas(
    const QSurfaceFormat & format
,   QScreen * screen)
: QWindow(screen)
, m_context(nullptr)
, m_painter(nullptr)
, m_camera(nullptr)
, m_navigation(nullptr)
, m_swapInterval(VerticalSyncronization)
, m_repaintTimer(new QBasicTimer())
, m_continuousRepaint(false)
, m_fpsTimer(nullptr)
, m_swapts(0.0)
, m_swaps(0)
, m_update(false)
{
    setSurfaceType(OpenGLSurface);    
    create();

    m_camera = new Camera();
    m_camera->setFovy(40.0);

    m_navigation = new Navigation(*m_camera);
    m_navigation->reset();

    initializeGL(format);
}

Canvas::~Canvas()
{
	delete m_repaintTimer;
    delete m_fpsTimer;

    delete m_navigation;
    delete m_camera;

	delete m_context;
}

QSurfaceFormat Canvas::format() const
{
    if (!m_context)
        return QSurfaceFormat();

    return m_context->format();
}

void Canvas::setContinuousRepaint(
    bool enable
,   int msec)
{
    if (m_continuousRepaint)
        m_repaintTimer->stop();

	m_continuousRepaint = enable;

    if (m_continuousRepaint)
        m_repaintTimer->start(msec, this);
}

bool Canvas::continuousRepaint() const
{
	return m_continuousRepaint;
}

const QString Canvas::querys(const GLenum penum) 
{
    const QString result = reinterpret_cast<const char*>(glGetString(penum));
    //glError();

    return result;
}

const GLint Canvas::queryi(const GLenum penum)
{
    GLint result;
    glGetIntegerv(penum, &result);

    return result;
}

void Canvas::initializeGL(const QSurfaceFormat & format)
{
    if (m_context)
        return;

    m_context = new QOpenGLContext;

    m_context->setFormat(format);
    m_context->create();

    m_context->makeCurrent(this);
    initializeOpenGLFunctions();

    // print some hardware information

    qDebug();
    qDebug().nospace() << "GPU: " 
        << qPrintable(querys(GL_RENDERER)) << " ("
        << qPrintable(querys(GL_VENDOR)) << ", "
        << qPrintable(querys(GL_VERSION)) << ")";
    qDebug().nospace() << "GL Version: "
        << qPrintable(QString::number(queryi(GL_MAJOR_VERSION))) << "."
        << qPrintable(QString::number(queryi(GL_MINOR_VERSION))) << " "
        << (queryi(GL_CONTEXT_CORE_PROFILE_BIT) ? "Core" : "Compatibility");
    qDebug();

    verifyExtensions(); // false if no painter ...

    connect(m_camera, &Camera::changed, this, &Canvas::cameraChanged);
}

void Canvas::resizeEvent(QResizeEvent * event)
{
    if (!m_context || !m_painter)
        return;

    m_camera->setViewport(event->size());

    m_context->makeCurrent(this);
    m_painter->resize(event->size().width(), event->size().height());
    m_context->doneCurrent();

    if (isExposed() && Hidden != visibility())
        paintGL();
}

void Canvas::paintGL()
{
    if (!m_painter || !isExposed() || Hidden == visibility())
        return;

    m_context->makeCurrent(this);
    auto programsWithInvalidatedUniforms(FileAssociatedShader::process()); // recompile file associated shaders if required

    if (m_update)
    {
        m_painter->update();
        m_update = false;
    }
    else
        m_painter->update(programsWithInvalidatedUniforms);
    m_painter->paint();

    m_context->swapBuffers(this);
    m_context->doneCurrent();


    if (!m_fpsTimer)
    {
        m_fpsTimer = new Timer(true, false);
        m_swapts = 0.0;
    }
    else
        m_fpsTimer->update();

    ++m_swaps;

    if (m_fpsTimer->elapsed() - m_swapts >= 1e+9)
    {
        const float fps = 1e+9f * static_cast<float>(static_cast<long double>
            (m_swaps) / (m_fpsTimer->elapsed() - m_swapts));

        emit fpsUpdate(fps);

        m_swapts = m_fpsTimer->elapsed();
        m_swaps = 0;
    }
}

void Canvas::cameraChanged()
{
    m_update = true;
}

void Canvas::timerEvent(QTimerEvent * event)
{
    assert(m_repaintTimer);

    if(event->timerId() != m_repaintTimer->timerId())
        return;

    paintGL();
}

void Canvas::assignPainter(AbstractPainter * painter)
{
    if (m_painter == painter)
        return;

    m_context->makeCurrent(this);

    m_painter = painter;
    m_painter->initialize();
    m_painter->setCamera(m_camera);

    verifyExtensions();

    m_context->doneCurrent();

    m_navigation->setCoordinateProvider(m_painter);
}

bool Canvas::verifyExtensions() const
{
    if (!m_painter)
        return false;

    if (!m_context)
    {
        qWarning("Extensions cannot be veryfied due to uninitialized context.");
        return false;
    }
    if (!m_context->isValid())
    {
        qWarning("Extensions cannot be veryfied due to invalid context.");
        return false;
    }

    QStringList unsupported;

    const QStringList & extensions(m_painter->extensions());
    foreach(const QString & extension, extensions)
    if (!m_context->hasExtension(qPrintable(extension)))
        unsupported << extension;

    if (unsupported.isEmpty())
        return true;

    if (unsupported.size() > 1)
        qWarning("The following mandatory OpenGL extensions are not supported:");
    else
        qWarning("The following mandatory OpenGL extension is not supported:");

    foreach(const QString & extension, unsupported)
        qWarning(qPrintable(extension));

    qWarning("");

    return false;
}

void Canvas::setSwapInterval(SwapInterval swapInterval)
{
    m_context->makeCurrent(this);

    bool result(false);
    m_swapInterval = swapInterval;

#ifdef WIN32

    typedef bool(WINAPI * SWAPINTERVALEXTPROC) (int);
    static SWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

    if (!wglSwapIntervalEXT)
        wglSwapIntervalEXT = reinterpret_cast<SWAPINTERVALEXTPROC>(m_context->getProcAddress("wglSwapIntervalEXT"));
    if (wglSwapIntervalEXT)
        result = wglSwapIntervalEXT(swapInterval);

#elif __APPLE__

    qWarning("ToDo: Setting swap interval is currently not implemented for __APPLE__");

#else

    typedef int(APIENTRY * SWAPINTERVALEXTPROC) (int);
    static SWAPINTERVALEXTPROC glXSwapIntervalSGI = nullptr;

    if (!glXSwapIntervalSGI)
        glXSwapIntervalSGI = reinterpret_cast<SWAPINTERVALEXTPROC>(m_context->getProcAddress("glXSwapIntervalSGI"));
    if (glXSwapIntervalSGI)
        result = glXSwapIntervalSGI(requestedInterval);

#endif

    if (!result)
        qWarning("Setting swap interval to %s failed."
            , qPrintable(swapIntervalToString(swapInterval)));
    else
        qDebug("Setting swap interval to %s."
            , qPrintable(swapIntervalToString(swapInterval)));

    m_context->doneCurrent();
}

void Canvas::toggleSwapInterval()
{
    switch (m_swapInterval)
    {
    case NoVerticalSyncronization:
        setSwapInterval(VerticalSyncronization);
        break;
    case VerticalSyncronization:
        setSwapInterval(AdaptiveVerticalSyncronization);
        break;
    case AdaptiveVerticalSyncronization:
        setSwapInterval(NoVerticalSyncronization);
        break;
    }
}

const QString Canvas::swapIntervalToString(SwapInterval swapInterval)
{
    switch (swapInterval)
    {
    case NoVerticalSyncronization:
        return QString("NoVerticalSyncronization");
    case VerticalSyncronization:
        return QString("VerticalSyncronization");
    case AdaptiveVerticalSyncronization:
        return QString("AdaptiveVerticalSyncronization");
    default:
        return QString();
    }
}


void Canvas::keyPressEvent(QKeyEvent * event)
{
    if (!m_navigation)
        return;

    m_navigation->keyPressEvent(event);
}
void Canvas::keyReleaseEvent(QKeyEvent * event)
{
    if (!m_navigation)
        return;

    m_navigation->keyReleaseEvent(event);
}

void Canvas::mouseMoveEvent(QMouseEvent * event)
{
    if (!m_navigation)
        return;

    m_context->makeCurrent(this);
    m_navigation->mouseMoveEvent(event);
    m_context->doneCurrent();
}

void Canvas::mousePressEvent(QMouseEvent * event)
{
    if (!m_navigation)
        return;

    m_context->makeCurrent(this);
    m_navigation->mousePressEvent(event);
    m_context->doneCurrent();
}

void Canvas::mouseReleaseEvent(QMouseEvent * event)
{
    if (!m_navigation)
        return;

    m_context->makeCurrent(this);
    m_navigation->mouseReleaseEvent(event);
    m_context->doneCurrent();
}

void Canvas::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (!m_navigation)
        return;

    m_context->makeCurrent(this);
    m_navigation->mouseDoubleClickEvent(event);
    m_context->doneCurrent();
}

void Canvas::wheelEvent(QWheelEvent * event)
{
    if (!m_navigation)
        return;

    m_context->makeCurrent(this);
    m_navigation->wheelEvent(event);
    m_context->doneCurrent();
}
