
#pragma once

#include <QMainWindow>
#include <QScopedPointer>


class Ui_Viewer;

class QLabel;
class QSurfaceFormat;
class QShortcut;

class Canvas;
class CyclicTime;
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
    void toggleAdaptiveGrid();
    void on_toggleTimeAction_triggered(bool checked);
    void toggleTime();

    void on_restartTimeAction_triggered(bool checked);
    void on_quitAction_triggered(bool checked);

protected:
    void setup();
    void setupCanvas(const QSurfaceFormat & format);

    void store();
    void restore();

    void updateAfterFullScreenToggle();

protected:
	const QScopedPointer<Ui_Viewer> m_ui;

    Canvas * m_canvas;
    AbstractPainter * m_painter;

    CyclicTime * m_time;

    QLabel * m_fpsLabel;
    QLabel * m_mouseLabel;
    QLabel * m_timeLabel;
    QLabel * m_objLabel;

    QScopedPointer<QShortcut> m_fullscreenShortcut;
    QScopedPointer<QShortcut> m_swapIntervalShortcut;
    QScopedPointer<QShortcut> m_adaptiveGridShortcut;
    QScopedPointer<QShortcut> m_toggleTimeShortcut;
};
