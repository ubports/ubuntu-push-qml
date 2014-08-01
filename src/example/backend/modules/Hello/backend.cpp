#include <QtQml>
#include <QtQml/QQmlContext>
#include "backend.h"
#include "../chatclient.h"


void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("hello"));

    qmlRegisterType<ChatClient>(uri, 1, 0, "ChatClient");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
