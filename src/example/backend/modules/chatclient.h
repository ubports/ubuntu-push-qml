#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

class ChatClient : public QObject
{
    Q_OBJECT
public:
    explicit ChatClient(QObject *parent = 0);
    QString nick;
    Q_PROPERTY(QString nick MEMBER nick);

signals:
    void error(QString);
    void registered();

public slots:
    void sendMessage(QJsonObject message, bool annoying);
    void registerNick(QString nick, QString token);
    void registerFinished();
    void handleError(QNetworkReply::NetworkError code);

private:
    QString token;
    QNetworkAccessManager *nam;
};

#endif // CHATCLIENT_H
