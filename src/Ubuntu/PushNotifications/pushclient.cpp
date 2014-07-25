#include "pushclient.h"
#include <QDebug>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QTimer>

#define PUSH_SERVICE "com.ubuntu.PushNotifications"
#define POSTAL_SERVICE "com.ubuntu.Postal"
#define PUSH_PATH "/com/ubuntu/PushNotifications"
#define POSTAL_PATH "/com/ubuntu/Postal"
#define PUSH_IFACE "com.ubuntu.PushNotifications"
#define POSTAL_IFACE "com.ubuntu.Postal"

PushClient::PushClient(QObject *parent) :
    QObject(parent)
{
}

void PushClient::registerApp(QString appid) {
    this->appid = appid;

    pkgname = appid.split("_").at(0);
    pkgname = pkgname.replace(".","_2e").replace("-","_2f");

    QString register_path(PUSH_PATH);
    register_path += "/" + pkgname;

    qDebug() << "registering:" << appid;
    QDBusConnection bus = QDBusConnection::sessionBus();

    // Register to the push client
    QDBusMessage message = QDBusMessage::createMethodCall(PUSH_SERVICE, register_path , PUSH_IFACE, "Register");
    message << appid;
    QDBusMessage token = bus.call(message);
    if (token.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Error registering:" << token.errorMessage();
        status = token.errorMessage();
        // This has to be delayed because the error signal is not connected yet
        QTimer::singleShot(200, this, SLOT(emitError()));
        return;
    }
    // Usually we would not show this ;-)
    qDebug() << "token" << token.arguments()[0].toStringList()[0];
    this->token = token.arguments()[0].toStringList()[0];

    // Connect to the notification signal
    QString postal_path(POSTAL_PATH);
    postal_path += "/" + pkgname;
    qDebug() << postal_path << "-----------";
    qDebug() << "connecting:" << bus.connect(POSTAL_SERVICE, postal_path, POSTAL_IFACE, "Post", "s", this, SLOT(notified(QString)));

    // Do an initial fetch
    QTimer::singleShot(200, this, SLOT(getNotifications()));
}

void PushClient::emitError()
{
    emit error(status);
}

void PushClient::notified(QString)
{
    qDebug() <<  "Notified";
    this->getNotifications();
}

void PushClient::getNotifications() {
    QDBusConnection bus = QDBusConnection::sessionBus();

    // FIXME: make async using http://qt-project.org/doc/qt-4.8/qdbusconnection.html#asyncCall
    QString path(POSTAL_PATH);
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "PopAll");
    message << this->appid;
    QDBusMessage reply = bus.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Error fetching notifications:" << reply.errorMessage();
        emit error(reply.errorMessage());
    }
    qDebug() << "notifications:" << reply.arguments()[0].toStringList();
    emit newNotifications(reply.arguments()[0].toStringList());
}
