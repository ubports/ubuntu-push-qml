/*
Copyright 2014 Canonical Ltd.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License, version 3
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this program.  If not, see
<http://www.gnu.org/licenses/>.
*/

#include "pushclient.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingCall>
#include <QtDBus/QDBusPendingReply>
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

void PushClient::registerApp(const QString &appId) {
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
    QDBusPendingCall pcall = bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                  this, SLOT(registerFinished(QDBusPendingCallWatcher*)));

    // Connect to the notification signal
    QString postal_path(POSTAL_PATH);
    postal_path += "/" + pkgname;
    bus.connect(POSTAL_SERVICE, postal_path, POSTAL_IFACE, "Post", "s", this, SLOT(notified(QString)));
}

void PushClient::registerFinished(QDBusPendingCallWatcher *watcher) {
    QDBusPendingReply<QString> reply = *watcher;
    if (reply.isError()) {
        status = reply.error().message();
        emit statusChanged(status);
        // This has to be delayed because the error signal is not connected yet
        QTimer::singleShot(200, this, SLOT(emitError()));
    }
    else {
        this->token = reply.value();
        // Do an initial fetch
        QTimer::singleShot(200, this, SLOT(getNotifications()));
        emit tokenChanged(this->token);
    }
}

QString PushClient::getAppId() {
    return appId;
}

QString PushClient::getToken() {
    return token;
}

void PushClient::emitError()
{
    emit error(status);
}

void PushClient::notified(const QString &)
{
    this->getNotifications();
}

void PushClient::getNotifications() {
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(POSTAL_PATH);
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "PopAll");
    message << this->appId;
    QDBusPendingCall pcall = bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                  this, SLOT(popAllFinished(QDBusPendingCallWatcher*)));
}

void PushClient::popAllFinished(QDBusPendingCallWatcher *watcher) {
    QDBusPendingReply<QStringList> reply = *watcher;
    if (reply.isError()) {
        emit error(reply.error().message());
    }
    else {
        emit notificationsChanged(reply.value());
    }
}

QStringList PushClient::getPersistent() {
    // FIXME: this is blocking, but making it async would change the API
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

void PushClient::clearPersistent(const QStringList &tags) {
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(POSTAL_PATH);
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "ClearPersistent");
    message << this->appId;
    for (int i = 0; i < tags.size(); ++i) {
		message << tags.at(i);
	}
    QDBusPendingCall pcall = bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                  this, SLOT(clearPersistentFinished(QDBusPendingCallWatcher*)));
}

void PushClient::clearPersistentFinished(QDBusPendingCallWatcher *watcher) {
    QDBusPendingReply<void> reply = *watcher;

    if (reply.isError()) {
        emit error(reply.error().message());
    } else {
        // FIXME: this is blocking
        emit persistentChanged(getPersistent());
    }
}

void PushClient::setCount(int count) {
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path(POSTAL_PATH);
    bool visible = count != 0;
    counter = count;
    path += "/" + pkgname;
    QDBusMessage message = QDBusMessage::createMethodCall(POSTAL_SERVICE, path, POSTAL_IFACE, "setCounter");
    message << this->appId << count << visible;
    QDBusPendingCall pcall = bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                  this, SLOT(setCounterFinished(QDBusPendingCallWatcher*)));
}

void PushClient::setCounterFinished(QDBusPendingCallWatcher *watcher) {
    QDBusPendingReply<void> reply = *watcher;
    if (reply.isError()) {
        emit error(reply.error().message());
    }
    else {
        emit countChanged(counter);
    }
}

int PushClient::getCount() {
    return counter;
}
