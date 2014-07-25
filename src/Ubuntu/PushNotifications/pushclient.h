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

    Q_PROPERTY(QString appid WRITE registerApp MEMBER appid);
    Q_PROPERTY(QString token MEMBER token);
    Q_PROPERTY(QStringList notifications NOTIFY newNotifications MEMBER notifications);
    Q_PROPERTY(QString status READ getStatus);

signals:
    void newNotifications(QStringList);
    void error(QString);

public slots:
    void getNotifications();
    void notified(QString appid);
    void emitError();

private:
    QString appid;
    QString pkgname;
    QString token;
    QString status;
    QStringList notifications;
};

#endif // PUSHCLIENT_H
