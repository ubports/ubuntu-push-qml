#ifndef PUSHCLIENT_H
#define PUSHCLIENT_H

#include <QObject>
#include <QString>
#include <QStringList>

class PushClient : public QObject
{
    Q_OBJECT
public:
    explicit PushClient(QObject *parent = 0);
    void registerApp(QString appid);
    QString getStatus() {return this->status;};
    QString getAppId();
    QString getToken();
    QStringList getPersistent();
    void clearPersistent(QStringList tags);
    void setCounter(int count, bool visible);

    Q_PROPERTY(QString appId WRITE registerApp READ getAppId NOTIFY appIdChanged);
    Q_PROPERTY(QString token READ getToken NOTIFY tokenChanged);
    Q_PROPERTY(QStringList notifications NOTIFY newNotifications);
    Q_PROPERTY(QString status READ getStatus NOTIFY statusChanged);
    Q_PROPERTY(QStringList persistent READ getPersistent);

signals:
    void newNotifications(QStringList);
    void appIdChanged(QString);
    void error(QString);
    void tokenChanged(QString);
    void statusChanged(QString);

public slots:
    void getNotifications();
    void notified(QString appId);
    void emitError();

private:
    QString appId;
    QString pkgname;
    QString token;
    QString status;
    QStringList notifications;
};

#endif // PUSHCLIENT_H
