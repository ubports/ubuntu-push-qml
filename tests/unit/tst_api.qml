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

    function test_a_pushClient() {
        compare(pushClient.appid, "")
        compare(pushClient.token, "")
        compare(pushClient.notifications, [])
    }

    function test_b_invalid() {
        var testId = "äöüß"
        pushClient.appid = testId
        compare(pushClient.status, "Invalid object path: /com/ubuntu/PushNotifications/" + testId)
    }

    function test_c_basics() {
        var testId = "com.ubuntu.developer.push.a_a"
        pushClient.appid = testId
        compare(pushClient.appid, testId)
    }
}
