import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1
import Ubuntu.PushNotifications 0.1
import Ubuntu.ChatClientPushExample 0.1

TestCase {
    name: "HelloComponent"

    ChatClient {
        id: chatClient
        nick: "b"
    }

    function init() {
    }

    function cleanup() {
    }

    function initTestCase() {
    }

    function cleanupTestCase() {
    }

    function test_canReadAndWriteText() {
        compare(chatClient.nick, "b")
    }
}
