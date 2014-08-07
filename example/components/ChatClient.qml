import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    property string nick
    property string token
    property bool registered: false
    signal error (string msg)
    onNickChanged: {register()}
    onTokenChanged: {register()}
    function register() {
        if (nick && token) {
            console.log("registering ", nick, token);

            var req = new XMLHttpRequest();
            req.open("post", "http://direct.ralsina.me:8001/register", true);
            req.setRequestHeader("Content-type", "application/json");
            req.onreadystatechange = function() {//Call a function when the state changes.
                if(req.readyState == 4) {
                    if (req.status == 200) {
                        registered = true;
                    } else {
                        error(JSON.parse(req.responseText)["error"]);
                    }
                }
            }
            req.send(JSON.stringify({
                "nick" : nick,
                "token": token
            }))
        }
    }
    function sendMessage(message, annoying) {
        var to_nick = message["to"]
        var data = {
            "from_nick": nick,
            "from_token": token,
            "nick": to_nick,
            "data": {
                "message": message,
                "notification": {}
            }
        }
        if (annoying) {
            data["notification"] = {
                "card": {
                    "summary": nick + " says: " + message["message"],
                    "body": "",
                    "popup": true,
                    "persist": true,
                    "actions": ["appid://com.ubuntu.developer.push.hello/hello/current-user-version"]
                }
            }
        }

        var req = new XMLHttpRequest();
        req.open("post", "http://direct.ralsina.me:8001/message", true);
        req.setRequestHeader("Content-type", "application/json");
        req.onreadystatechange = function() {//Call a function when the state changes.
            if(req.readyState == 4) {
                if (req.status == 200) {
                    registered = true;
                } else {
                    error(JSON.parse(req.responseText)["error"]);
                }
            }
        }
        req.send(JSON.stringify(data))
    }
}
