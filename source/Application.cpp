
#include <QDebug>
#include <QFileInfo>
#include <QStringList>

#include "version.h"

#include "MessageHandler.h"

#include "Application.h"


Application::Application(
    int & argc
,   char ** argv)
: QApplication(argc, argv)
{
	const QFileInfo fi(QCoreApplication::applicationFilePath());

	//QApplication::setApplicationDisplayName(fi.baseName());

	QApplication::setApplicationName(SANDBOX_PROJECT_NAME);
	QApplication::setApplicationVersion(SANDBOX_VERSION);

	QApplication::setOrganizationName(SANDBOX_AUTHOR_ORGANIZATION);
	QApplication::setOrganizationDomain(SANDBOX_AUTHOR_DOMAIN);

	qInstallMessageHandler(globalMessageHandler);

	qDebug("%s on Qt-%s", qPrintable(title()), qPrintable(qVersion()));
	qDebug() << qPrintable(SANDBOX_VERSION);
}

Application::~Application()
{
    qDebug("%s exited", qPrintable(title()));
    qDebug();
}

const QString Application::title()
{
    return QApplication::applicationName();
}
