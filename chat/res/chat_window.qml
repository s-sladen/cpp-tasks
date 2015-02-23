import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import ChatModule 1.0

Rectangle {
    anchors.fill: parent

    TextArea {
        id: messages
        readOnly: true
        textFormat: TextEdit.RichText
        verticalAlignment: Text.AlignBottom
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: users.left
        anchors.bottom: message.top
    }

    function appendMessage(message) {
        var wasAtBottom = messages.flickableItem.contentY == messages.flickableItem.contentHeight - messages.flickableItem.height;
        messages.text += message;
        if (wasAtBottom) {
            messages.flickableItem.contentY = messages.flickableItem.contentHeight - messages.flickableItem.height;
        }
    }

    ListView {
        id: users

        width: 200
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: message.top

        model: chat.users
        delegate: Text {
            text: modelData
        }
    }

    TextField {
        id: message
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        placeholderText : qsTr("Enter message...")

        onAccepted: {
            if (text.trim() !== "") {
                chat.sendMessage(text);
                text = "";
            }
        }
    }

    Chat {
        id: chat
        onLoggedIn: appendMessage("<font color='green'>" + nickname + " logged in</font><br/>");
        onMessageCame: appendMessage(author + ": " + message + "<br/>");
        onLoggedOut: appendMessage("<font color='red'>" + nickname + " logged out</font><br/>");
    }

    Dialog {
        id: nicknameDialog
        width: 300
        visible: true
        title: qsTr("Choose nickname")
        standardButtons: StandardButton.Apply | StandardButton.Cancel

        onAccepted: chat.login(nickname.text)
        onRejected: Qt.quit()

        Label {
            id: label
            text: qsTr("Enter nickname: ")
        }

        TextField {
            id: nickname
            validator: RegExpValidator { regExp: /\w{2,20}/ }
            anchors.top: label.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}