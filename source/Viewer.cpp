
#include <sstream>
#include <iomanip>
#include <cassert>


#include <QOpenGLContext>
#include <QShortcut>
#include <QSettings>
#include <QLabel>
#include <QVector3D>

#include "Application.h"
#include "Canvas.h"
#include "Painter.h"
#include "CyclicTime.h"
#include "Viewer.h"

#include "ui_Viewer.h"


namespace 
{
    const QString SETTINGS_GEOMETRY ("Geometry");
    const QString SETTINGS_STATE    ("State");
    
    const QString SETTINGS_ADAPTIVE_GRID("ShowAdaptiveGrid");
}

Viewer::Viewer(
    const QSurfaceFormat & format
,   QWidget * parent
,    Qt::WindowFlags flags)

: QMainWindow(parent, flags)
, m_ui(new Ui_Viewer)
, m_canvas(nullptr)
, m_painter(nullptr)
, m_time(new CyclicTime(0.0L, 60.0)) // this is one day in 60 seconds (1d/1h)
, m_fullscreenShortcut(nullptr)
, m_swapIntervalShortcut(nullptr)
, m_adaptiveGridShortcut(nullptr)
{
    m_ui->setupUi(this);
    setWindowTitle(Application::title());

    setup();
    setupCanvas(format);

    m_time->setf(0.0);
    m_time->start();

    restore();

    updateAfterFullScreenToggle();
};


Viewer::~Viewer()
{
    store();

    setCentralWidget(nullptr);

    m_canvas->setPainter(nullptr);
    delete m_painter;
    delete m_canvas;

    delete m_time;
}

void Viewer::restore()
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings s;

    restoreGeometry(s.value(SETTINGS_GEOMETRY).toByteArray());
    restoreState(s.value(SETTINGS_STATE).toByteArray());



    assert(m_canvas);
    bool enable = s.value(SETTINGS_ADAPTIVE_GRID, true).toBool();
    m_canvas->setAdaptiveGrid(enable);
    m_ui->showAdaptiveGridAction->setChecked(enable);
}

void Viewer::store()
{
    QSettings s;
    s.setValue(SETTINGS_GEOMETRY, saveGeometry());
    s.setValue(SETTINGS_STATE, saveState());

    assert(m_canvas);
    s.setValue(SETTINGS_ADAPTIVE_GRID, m_canvas->adaptiveGrid());
}

void Viewer::setup()
{
    // ToDo: this seems to be a generic problem (should be done by qt main window itself but....)
    //       We need to parse all available shortcuts via any menubars and connect those...

    m_fullscreenShortcut.reset(new QShortcut(m_ui->toggleFullScreenAction->shortcut(), this));
    connect(m_fullscreenShortcut.data(), &QShortcut::activated, this, &Viewer::toggleFullScreen);

    m_swapIntervalShortcut.reset(new QShortcut(m_ui->toggleSwapIntervalAction->shortcut(), this));
    connect(m_swapIntervalShortcut.data(), &QShortcut::activated, this, &Viewer::toggleSwapInterval);

    m_adaptiveGridShortcut.reset(new QShortcut(m_ui->showAdaptiveGridAction->shortcut(), this));
    connect(m_adaptiveGridShortcut.data(), &QShortcut::activated, this, &Viewer::toggleAdaptiveGrid);

    m_toggleTimeShortcut.reset(new QShortcut(m_ui->toggleTimeAction->shortcut(), this));
    connect(m_toggleTimeShortcut.data(), &QShortcut::activated, this, &Viewer::toggleTime);


    m_objLabel = new QLabel(m_ui->statusbar);
    m_ui->statusbar->addPermanentWidget(m_objLabel);
    m_mouseLabel = new QLabel(m_ui->statusbar);
    m_ui->statusbar->addPermanentWidget(m_mouseLabel);
    m_timeLabel = new QLabel(m_ui->statusbar);
    m_ui->statusbar->addPermanentWidget(m_timeLabel);
    m_fpsLabel = new QLabel(m_ui->statusbar);
    m_ui->statusbar->addPermanentWidget(m_fpsLabel);
}

void Viewer::setupCanvas(const QSurfaceFormat & format)
{
    m_canvas = new Canvas(format);
    m_canvas->setContinuousRepaint(true, 0);
    m_canvas->setSwapInterval(Canvas::VerticalSyncronization);

    m_canvas->setTime(m_time);

    connect(m_canvas, &Canvas::fpsUpdate, this, &Viewer::fpsChanged);
    connect(m_canvas, &Canvas::objUpdate, this, &Viewer::objChanged);
    connect(m_canvas, &Canvas::timeUpdate, this, &Viewer::timeChanged);
    connect(m_canvas, &Canvas::mouseUpdate, this, &Viewer::mouseChanged);

    m_painter = new Painter();
    m_canvas->setPainter(m_painter);

    QWidget * widget = QWidget::createWindowContainer(m_canvas);
    widget->setMinimumSize(1, 1);
    widget->setAutoFillBackground(false); // Important for overdraw, not occluding the scene.
    widget->setFocusPolicy(Qt::TabFocus);

    setCentralWidget(widget);
    show();
}

void Viewer::fpsChanged(float fps)
{
    m_fpsLabel->setText(QString(" %1 fps ")
        .arg(fps, 2, 'g', 4));
}

void Viewer::mouseChanged(const QPoint & mouse)
{
    m_mouseLabel->setText(QString(" %1 %2 mouse ")
        .arg(mouse.x(), 4, 10, QChar('0')).arg(mouse.y(), 4, 10, QChar('0')));
}

void Viewer::timeChanged(float timef)
{
    std::stringstream s;
    s << " " << std::setprecision(3) << std::setfill('0') << std::setw(3) << timef << " time ";

    m_timeLabel->setText(QString::fromStdString(s.str()));
}

void Viewer::objChanged(const QVector3D & obj)
{
    m_objLabel->setText(QString(" %1%2 %3%4 %5%6 xyz ")
        .arg(obj.x() > 0 ? " " : "").arg(obj.x())
        .arg(obj.y() > 0 ? " " : "").arg(obj.y())
        .arg(obj.z() > 0 ? " " : "").arg(obj.z()));
}

void Viewer::on_toggleFullScreenAction_triggered(bool checked)
{
    toggleFullScreen();
}

void Viewer::toggleFullScreen()
{
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();

    updateAfterFullScreenToggle();
}

void Viewer::updateAfterFullScreenToggle()
{
    m_ui->menubar->setVisible(isFullScreen());
    m_ui->statusbar->setVisible(isFullScreen());

    m_ui->menubar->setVisible(!isFullScreen());
    m_ui->statusbar->setVisible(!isFullScreen());
    m_fullscreenShortcut->setEnabled(isFullScreen());
    m_swapIntervalShortcut->setEnabled(isFullScreen());
    m_adaptiveGridShortcut->setEnabled(isFullScreen());
    m_toggleTimeShortcut->setEnabled(isFullScreen());
}

void Viewer::on_toggleSwapIntervalAction_triggered(bool checked)
{
    toggleSwapInterval();
}

void Viewer::toggleSwapInterval()
{
    assert(m_canvas);
    m_canvas->toggleSwapInterval();
}

void Viewer::on_showAdaptiveGridAction_triggered(bool checked)
{
    assert(m_canvas);
    m_canvas->setAdaptiveGrid(checked);
}

void Viewer::toggleAdaptiveGrid()
{
    assert(m_canvas);
    m_canvas->setAdaptiveGrid(!m_canvas->adaptiveGrid());
}

void Viewer::on_toggleTimeAction_triggered(bool checked)
{
    toggleTime();
}

void Viewer::toggleTime()
{
    if (!m_time->isRunning())
        m_time->start();
    else
        m_time->pause();
}

void Viewer::on_restartTimeAction_triggered(bool checked)
{
    const bool wasRunning(m_time->isRunning());
    m_time->reset();

    if (wasRunning)
        m_time->start();
}

void Viewer::on_quitAction_triggered(bool checked)
{
    QApplication::quit();
}
