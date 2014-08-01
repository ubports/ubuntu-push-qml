#include "chatclient.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>

#define REGISTER_URL  QUrl("http://direct.ralsina.me:8001/register")
#define POST_URL  QUrl("http://direct.ralsina.me:8001/message")

ChatClient::ChatClient(QObject *parent) :
    QObject(parent)
{
    this->nam = new QNetworkAccessManager(this);
}

void ChatClient::registerNick(QString _nick, QString _token) {
    qDebug() <<  "registering nick" << _nick << _token;
    nick = _nick.toLower();
    token = _token;
    QNetworkRequest req;
    req.setUrl(REGISTER_URL);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject json;
    // Create JSON representation, will look like this:
    //{
    //    "nick": "joe",
    //    "token": "asdasdasdasd=="
    //}

    json["nick"] = nick;
    json["token"] = token;
    QJsonDocument json_doc(json);
    qDebug() <<  "registering nick" << json_doc.toJson();
    QNetworkReply *reply = this->nam->post(req, json_doc.toJson());
    // Emit an error for network issues, emit registered for success
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(handleError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), SLOT(registerFinished()));
}

void ChatClient::sendMessage(QJsonObject message, bool annoying) {
    QString to_nick = message["to"].toString().toLower();
    // Replace sender with recipient, so recipient sees who is sending

    // Create JSON representation, will look like this:
    //{
    //    "from_nick": "alice",
    //    "from_token": "foobar==",
    //    "nick": "bob",
    //    "data": {
    //        "message": {
    //          "message": "hello",
    //          "from": "from_nick",
    //          "to": "to_nick",
    //          },
    //        "notification": {
    //            "card": {
    //                "summary": "nickname: hello",
    //                "body": "",
    //                "popup": true,
    //                "persist": true
    //            }
    //        }
    //    }
    //}
    QJsonObject json;
    QJsonObject notification;
    QJsonObject data;
    if (annoying) {
        QJsonObject card;
        card["summary"] = nick + " says: " + message["message"].toString();
        card["body"] = "";
        card["popup"] = true;
        card["persist"] = true;
        QJsonArray actions;
        actions.append("appid://com.ubuntu.developer.ralsina.hello/hello/current-user-version");
        card["actions"] = actions;
        notification["card"] = card;
    }
    data["message"] = message;
    data["notification"] = notification;
    json["data"] = data;
    json["nick"] = to_nick;
    json["from_nick"] = nick;
    json["from_token"] = this->token;
    QJsonDocument json_doc(json);

    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setUrl(POST_URL);
    qDebug() << "sending message to:" << to_nick << ":" << json_doc.toJson();
    QNetworkReply *reply = this->nam->post(req, json_doc.toJson());
    //Emit error for network issues, do nothing for success (no news is good news)
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(handleError(QNetworkReply::NetworkError)));
}

void ChatClient::handleError(QNetworkReply::NetworkError code) {
    // Convert network errors into strings and pass along.
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString jsonError = QJsonDocument::fromJson(reply->readAll()).object()["error"].toString();
    QString errormsg = QString("Chat server said: ") + reply->errorString() + "--" + jsonError;
    qDebug() << errormsg;
    emit error(errormsg);
    reply->deleteLater();
}

void ChatClient::registerFinished() {
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        emit registered();
    }
    reply->deleteLater();
}
