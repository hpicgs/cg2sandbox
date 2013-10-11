
#pragma once

#include <QWindow>
#include <QOpenGLFunctions_3_2_Core>

#include <QList>

class QOpenGLContext;
class QSurfaceFormat;
class QBasicTimer;
class QTimerEvent;
class QKeyEvent;

class AbstractPainter;
class Camera;
class Navigation;
class Timer;


class Canvas : public QWindow, protected QOpenGLFunctions_3_2_Core
{
    Q_OBJECT

public:
    enum SwapInterval
    {
        NoVerticalSyncronization        =  0
    ,   VerticalSyncronization          =  1 ///< WGL_EXT_swap_control, GLX_EXT_swap_control, GLX_SGI_video_sync
    ,   AdaptiveVerticalSyncronization  = -1 ///< requires EXT_swap_control_tear
    };

public:
    Canvas(
        const QSurfaceFormat & format
    ,   QScreen * screen = nullptr);
    virtual ~Canvas();

    // from QWindow
    virtual QSurfaceFormat format() const;

    void setContinuousRepaint(bool enable, int msec = 1000 / 60);
	bool continuousRepaint() const;

    void assignPainter(AbstractPainter * painter);
    AbstractPainter * painter();

    void setSwapInterval(SwapInterval swapInterval);
    static const QString swapIntervalToString(SwapInterval swapInterval);

public slots:
    void toggleSwapInterval();
    
protected slots:
    void cameraChanged();

protected:
    const QString querys(const GLenum penum);
    const GLint queryi(const GLenum penum);

    virtual void initializeGL(const QSurfaceFormat & format);
	virtual void paintGL();

	//virtual void showEvent(QShowEvent * event);
	//virtual void hideEvent(QHideEvent * event);

	virtual void resizeEvent(QResizeEvent * event);

    // navigation event forwarding

    virtual void keyPressEvent        (QKeyEvent   * event);
    virtual void keyReleaseEvent      (QKeyEvent   * event);

    virtual void mouseMoveEvent       (QMouseEvent * event);
    virtual void mousePressEvent      (QMouseEvent * event);
    virtual void mouseReleaseEvent    (QMouseEvent * event);
    virtual void mouseDoubleClickEvent(QMouseEvent * event);

    virtual void wheelEvent           (QWheelEvent * event);


    /** uses the context to verify existence/support of each mandatory extension
    */
    bool verifyExtensions() const;

	void timerEvent(QTimerEvent * event);

signals:
    void fpsUpdate(float fps);

protected:
    QOpenGLContext * m_context;

    AbstractPainter * m_painter;
    Camera * m_camera;
    Navigation * m_navigation;

    SwapInterval m_swapInterval;    ///< required for toggle

    QBasicTimer * m_repaintTimer;
    Timer * m_fpsTimer;

    long double m_swapts;
    unsigned int m_swaps;

    bool m_update; // checked in paintGL, if true, update of program gets triggered

    bool m_continuousRepaint;
};
