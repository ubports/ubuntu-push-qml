#include "plugin.h"
#include "pushclient.h"

// Qt
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QtGlobal>
#include <QtGui/QGuiApplication>
#include <QtQml>

void UbuntuBrowserPlugin::initializeEngine(QQmlEngine* engine, const char* uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

void UbuntuBrowserPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.PushNotifications"));

    qmlRegisterType<PushClient>(uri, 0, 1, "PushClient");
}
