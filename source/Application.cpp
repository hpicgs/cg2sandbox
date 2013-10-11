#ifndef NDEBUG
	#include <QDebug>
#endif

#include "version.h"

#include "MessageHandler.h"

#include "Application.h"


Application::Application(
    int & argc
,   char ** argv)
: QApplication(argc, argv)
{
	QApplication::setApplicationName(SANDBOX_PROJECT_NAME);
	QApplication::setApplicationVersion(SANDBOX_VERSION);

	QApplication::setOrganizationName(SANDBOX_AUTHOR_ORGANIZATION);
	QApplication::setOrganizationDomain(SANDBOX_AUTHOR_DOMAIN);

	qInstallMessageHandler(globalMessageHandler);

#ifndef NDEBUG
	qDebug("%s on Qt-%s", qPrintable(title()), qPrintable(qVersion()));
	qDebug() << qPrintable(SANDBOX_VERSION);
#endif
}

Application::~Application()
{
#ifndef NDEBUG
    qDebug("%s exited", qPrintable(title()));
    qDebug();
#endif
}

const QString Application::title()
{
    return QString("%1 %2")
        .arg(QApplication::applicationName())
        .arg(QApplication::applicationVersion());
}
