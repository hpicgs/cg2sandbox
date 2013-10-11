
#include <sstream>
#include <iomanip>


#include <QOpenGLContext>
#include <QShortcut>
#include <QSettings>
#include <QLabel>
#include <QVector3D>

#include "Application.h"
#include "Viewer.h"

#include "ui_viewer.h"


namespace 
{
    const QString SETTINGS_GEOMETRY ("Geometry");
    const QString SETTINGS_STATE    ("State");
}

Viewer::Viewer(
    QWidget * parent,
    Qt::WindowFlags flags)

: QMainWindow(parent, flags)
, m_ui(new Ui_Viewer)
, m_fullscreenShortcut(nullptr)
, m_swapIntervalShortcut(nullptr)
{
    m_ui->setupUi(this);
    setWindowTitle(Application::title());

    m_fullscreenShortcut = new QShortcut(m_ui->toggleFullScreenAction->shortcut(), this);
    connect(m_fullscreenShortcut, &QShortcut::activated, this, &Viewer::toggleFullScreen);

    m_swapIntervalShortcut = new QShortcut(m_ui->toggleSwapIntervalAction->shortcut(), this);
    connect(m_swapIntervalShortcut, &QShortcut::activated, this, &Viewer::toggleSwapInterval);

    m_objLabel = new QLabel("obj", m_ui->statusbar);
    m_ui->statusbar->addPermanentWidget(m_objLabel);
    m_mouseLabel = new QLabel("mouse", m_ui->statusbar);
    m_ui->statusbar->addPermanentWidget(m_mouseLabel);
    m_fpsLabel = new QLabel("fps", m_ui->statusbar);
    m_ui->statusbar->addPermanentWidget(m_fpsLabel);


    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings s;

    restoreGeometry(s.value(SETTINGS_GEOMETRY).toByteArray());
    restoreState(s.value(SETTINGS_STATE).toByteArray());

    m_ui->menubar->setVisible(!isFullScreen());
    m_ui->statusbar->setVisible(!isFullScreen());
    m_fullscreenShortcut->setEnabled(isFullScreen());
    m_swapIntervalShortcut->setEnabled(isFullScreen());
};

Viewer::~Viewer()
{
    QSettings s;
    s.setValue(SETTINGS_GEOMETRY, saveGeometry());
    s.setValue(SETTINGS_STATE, saveState());
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
    m_ui->menubar->setVisible(isFullScreen());
    m_ui->statusbar->setVisible(isFullScreen());

    if (isFullScreen())
        showNormal();
    else
        showFullScreen();

    m_fullscreenShortcut->setEnabled(isFullScreen());
    m_swapIntervalShortcut->setEnabled(isFullScreen());
}

void Viewer::on_toggleSwapIntervalAction_triggered(bool checked)
{
    emit toggleSwapInterval();
}
