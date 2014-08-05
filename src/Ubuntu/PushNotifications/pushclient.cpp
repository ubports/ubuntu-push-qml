#include "pushclient.h"
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

void PushClient::registerApp(QString appId) {
    if (appId == this->appId || appId == "")
        return;

    this->appId = appId;

    pkgname = appId.split("_").at(0);
    pkgname = pkgname.replace(".","_2e").replace("-","_2d");
    emit appIdChanged(appId);

    QString register_path(PUSH_PATH);
    register_path += "/" + pkgname;

    QDBusConnection bus = QDBusConnection::sessionBus();

    // Register to the push client
    QDBusMessage message = QDBusMessage::createMethodCall(PUSH_SERVICE, register_path , PUSH_IFACE, "Register");
    message << appId;
    QDBusMessage token = bus.call(message);
    if (token.type() == QDBusMessage::ErrorMessage) {
        status = token.errorMessage();
        emit statusChanged(status);
        // This has to be delayed because the error signal is not connected yet
        QTimer::singleShot(200, this, SLOT(emitError()));
        return;
    }
    this->token = token.arguments()[0].toStringList()[0];

    // Connect to the notification signal
    QString postal_path(POSTAL_PATH);
    postal_path += "/" + pkgname;
    bus.connect(POSTAL_SERVICE, postal_path, POSTAL_IFACE, "Post", "s", this, SLOT(notified(QString)));

    // Do an initial fetch
    QTimer::singleShot(200, this, SLOT(getNotifications()));
    emit tokenChanged(this->token);
}

QString PushClient::getAppId() {
    return appId;
}

QString PushClient::getToken() {
    return appId;
}

void PushClient::emitError()
{
    emit error(status);
}

void PushClient::notified(QString)
{
    this->getNotifications();
}

void PushClient::getNotifications() {
    QDBusConnection bus = QDBusConnection::sessionBus();

    // FIXME: make async using http://qt-project.org/doc/qt-4.8/qdbusconnection.html#asyncCall
    QString path(POSTAL_PATH);
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "PopAll");
    message << this->appId;
    QDBusMessage reply = bus.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        emit error(reply.errorMessage());
    }
    emit notificationsChanged(reply.arguments()[0].toStringList());
}

QStringList PushClient::getPersistent() {
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(POSTAL_PATH);
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "ListPersistent");
    message << this->appId;
    QDBusMessage reply = bus.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        emit error(reply.errorMessage());
    }
    return reply.arguments()[0].toStringList();
}

void PushClient::clearPersistent(QStringList tags) {
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(POSTAL_PATH);
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "ClearPersistent");
    message << this->appId << tags;
    QDBusMessage reply = bus.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        emit error(reply.errorMessage());
    }
}

void PushClient::setCount(int count) {
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(POSTAL_PATH);
    bool visible = count != 0;
    this->counter = count;
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "setCounter");
    message << this->appId << count << visible;
    QDBusMessage reply = bus.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        emit error(reply.errorMessage());
    }
}

int PushClient::getCount() {
    return counter;
}