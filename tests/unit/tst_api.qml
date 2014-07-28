import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1
import Ubuntu.PushNotifications 0.1

TestCase {
    name: "API"

    PushClient {
        id: pushClient
        appid: ""
    }

    function init() {
    }

    function cleanup() {
    }

    function initTestCase() {
    }

    function cleanupTestCase() {
    }

    function test_defaults() {
        compare(pushClient.appid, "")
        compare(pushClient.status, "Invalid object path: /com/ubuntu/PushNotifications/")
        compare(pushClient.notifications, [])
    }
}
