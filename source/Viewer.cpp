
#include <sstream>
#include <iomanip>


#include <QOpenGLContext>
#include <QShortcut>
#include <QSettings>

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

    m_fullscreenShortcut = new QShortcut(m_ui->toggleFullScreenAction->shortcut(), this);
    connect(m_fullscreenShortcut, &QShortcut::activated, this, &Viewer::toggleFullScreen);

    m_swapIntervalShortcut = new QShortcut(m_ui->toggleSwapIntervalAction->shortcut(), this);
    connect(m_swapIntervalShortcut, &QShortcut::activated, this, &Viewer::toggleSwapInterval);


    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings s;

    restoreGeometry(s.value(SETTINGS_GEOMETRY).toByteArray());
    restoreState(s.value(SETTINGS_STATE).toByteArray());

    m_ui->menubar->setVisible(!isFullScreen());
    m_fullscreenShortcut->setEnabled(isFullScreen());
    m_swapIntervalShortcut->setEnabled(isFullScreen());
};

Viewer::~Viewer()
{
    delete m_fullscreenShortcut;
    delete m_swapIntervalShortcut;

    QSettings s;
    s.setValue(SETTINGS_GEOMETRY, saveGeometry());
    s.setValue(SETTINGS_STATE, saveState());
}

void Viewer::fpsChanged(float fps)
{
    std::stringstream fpss;
    fpss << Application::title().toStdString() << " (" 
        << std::fixed << std::setprecision(2) << fps << " fps)";

    setWindowTitle(QString::fromStdString(fpss.str()));
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

    m_ui->menubar->setVisible(!isFullScreen());

    m_fullscreenShortcut->setEnabled(isFullScreen());
    m_swapIntervalShortcut->setEnabled(isFullScreen());
}

void Viewer::on_toggleSwapIntervalAction_triggered(bool checked)
{
    emit toggleSwapInterval();
}
