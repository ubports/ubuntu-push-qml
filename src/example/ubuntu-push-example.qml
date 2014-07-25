import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.PushNotifications 0.1
import Ubuntu.ChatClientPushExample 0.1

/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "com.ubuntu.developer.push.hello"

    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    automaticOrientation: true

    width: units.gu(100)
    height: units.gu(75)
    ChatClient {
        id: chatClient
        onError: messageList.handle_error
        onRegistered: nickEdit.registered
    }

    PushClient {
        id: pushClient
        onNewNotifications: messageList.handle_notifications
        onError: messageList.handle_error
        appid: "com.ubuntu.developer.push.hello_hello"

    }

    TextField {
        id: nickEdit
        focus: true
        placeholderText: "Your nickname"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: units.gu(.5)
        anchors.rightMargin: units.gu(.5)
        anchors.topMargin: units.gu(.5)
        function registered() {
            readOnly = true
            text = "Your nick is " + chatClient.nick
            messageEdit.focus = true
        }
        onAccepted: chatClient.registerNick(text, pushClient.token)
    }
    TextField {
        id: messageEdit
        anchors.right: annoyingSwitch.left
        anchors.left: parent.left
        anchors.top: nickEdit.bottom
        anchors.topMargin: units.gu(1)
        anchors.rightMargin: units.gu(1)
        anchors.leftMargin: units.gu(.5)
        placeholderText: "Your message"
        onAccepted: {
            console.log("sending " + text)
            messagesModel.insert(0, {
                "from" :  chatClient.nick,
                "to" :  "",
                "type": "sent",
                "message" : text
            })
            chatClient.sendMessage(text, annoyingSwitch.checked)
            text = ""
        }
    }
    Switch {
        id: annoyingSwitch
        anchors.right: parent.right
        anchors.top: nickEdit.bottom
        anchors.topMargin: units.gu(1)
        anchors.rightMargin: units.gu(.5)
    }

    ListModel {
        id: messagesModel
        ListElement {
            from: ""
            to: ""
            type: "info"
            message: "Register by typing your nick and pressing enter."
        }
        ListElement {
            from: ""
            to: ""
            type: "info"
            message: "Send messages in the form \"destination: hello\""
        }
        ListElement {
            from: ""
            to: ""
            type: "info"
            message: "The switch on the right makes the message more annoying for the recipient."
        }
    }

    UbuntuShape {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: messageEdit.bottom
        anchors.topMargin: units.gu(1)
        ListView {
            id: messageList
            model: messagesModel
            anchors.fill: parent
            delegate: Rectangle {
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (from != "") {
                            messageEdit.text = from + ": "
                            messageEdit.focus = true
                        }
                    }
                }
                height: label.height + units.gu(2)
                width: parent.width
                Rectangle {
                    color: {
                        "info": "#E0E7AB",
                        "received" : "#A2CFA5",
                        "sent" : "#00FF00",
                        "error" : "#FF0000"}[type]
                    height: label.height + units.gu(1)
                    anchors.fill: parent
                    radius: 5
                    anchors.margins: units.gu(.5)
                    Text {
                        id: label
                        text: "<b>" + from + (from?":":"") + "</b> " + message // FIXME: make better
                        wrapMode: Text.Wrap
                        width: parent.width - units.gu(1)
                        x: units.gu(.5)
                    }
                }
            }

            function handle_error(error) {
                messagesModel.insert(0, {
                     "from" :  "",
                     "to" :  "",
                     "type" :  "error",
                     "message" : "<b>ERROR: " + error + "</b>"
                })
            }

            function handle_notifications(list) {
                list.forEach(function(notification) {
                    var item = JSON.parse(notification)
                    item["type"] = "received"
                    messagesModel.insert(0, item)
                })
            }
        }
    }
}
