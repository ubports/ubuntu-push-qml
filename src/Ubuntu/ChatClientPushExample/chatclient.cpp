#include "chatclient.h"

#include <QDebug>
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
    connect(reply, SIGNAL(finished()), SIGNAL(registered()));
}

void ChatClient::sendMessage(QString msg, bool annoying) {
    // msg is a json object
    QJsonDocument msg_doc=QJsonDocument::fromJson(msg.toUtf8());
    if (!msg_doc.isObject()) {
        emit error("Malformed message in sendMessage.");
    }
    QJsonObject message = msg_doc.object();

    QString to_nick = message["to"].toString().toLower();
    // Replace sender with recipient, so recipient sees who is sending
    QString realmsg = nick + " says: " + message["text"].toString().toLower().trimmed();

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
        card["summary"] = realmsg;
        card["body"] = "";
        card["popup"] = true;
        card["persist"] = true;
        notification["card"] = card;
    }
    message["text"] = realmsg;
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
    qDebug() << "HTTP error:" << reply->errorString();
    emit error(QString("Chat server said: ") + reply->errorString());
    reply->deleteLater();
}
