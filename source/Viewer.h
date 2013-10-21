
#pragma once

#include <QMainWindow>
#include <QScopedPointer>


class Ui_Viewer;

class QLabel;
class QSurfaceFormat;
class QShortcut;

class Canvas;
class AbstractPainter;


class Viewer : public QMainWindow
{
    Q_OBJECT

public:
    Viewer(
        const QSurfaceFormat & format
    ,   QWidget * parent = nullptr
    ,   Qt::WindowFlags flags = NULL);

    virtual ~Viewer();

public slots:
    void fpsChanged(float fps);
    void mouseChanged(const QPoint & mouse);
    void objChanged(const QVector3D & obj);
    void timeChanged(float timef);

protected slots:
    void on_toggleFullScreenAction_triggered(bool checked);
    void toggleFullScreen();

    void on_toggleSwapIntervalAction_triggered(bool checked);
    void toggleSwapInterval();

    void on_showAdaptiveGridAction_triggered(bool checked);

    void on_quitAction_triggered(bool checked);

protected:
    void setup();
    void setupCanvas(const QSurfaceFormat & format);

    void store();
    void restore();

protected:
	const QScopedPointer<Ui_Viewer> m_ui;

    Canvas * m_canvas;
    AbstractPainter * m_painter;

    QLabel * m_fpsLabel;
    QLabel * m_mouseLabel;
    QLabel * m_timeLabel;
    QLabel * m_objLabel;

    QShortcut * m_fullscreenShortcut;
    QShortcut * m_swapIntervalShortcut;
};
