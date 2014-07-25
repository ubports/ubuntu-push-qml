#ifndef __PLUGIN_H__
#define __PLUGIN_H__

// Qt
#include <QtQml/QQmlExtensionPlugin>

class UbuntuBrowserPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void initializeEngine(QQmlEngine* engine, const char* uri);
    void registerTypes(const char* uri);
};

#endif // __PLUGIN_H__
