import QtQml 2.12
import QtQuick 2.15
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15


ApplicationWindow {
    width: 400
    height: 400
    visible: true
    title: qsTr("Simple Log Viewer")

    property bool snap: true

    Connections {
        target: LogModel
        function onNewRows() {
            if (snap)
                scrollBar.position = 1.0
        }
    }

    Connections {
        target: Serial
        function onMessage(text) {
            footerText.text = text
        }
    }

    footer: Text {
        id: footerText
    }

    ListView {
        anchors.fill: parent
        model: LogModel
        boundsBehavior: Flickable.StopAtBounds
        delegate: TextArea {
            padding: 0
            width: ListView.view.width
            textFormat: TextEdit.AutoText
            wrapMode: Text.Wrap
            text: display
            background: Rectangle {
                gradient: Gradient {
                             GradientStop { position: 0; color: "#ffffff" }
                             GradientStop { position: 1; color: "#c1bbf9" }
                }
            }
        }

        onAtYEndChanged: {
            if (atYEnd)
                snap = true

            if (!atYEnd && scrollBar.pressed)
                snap = false
        }

        onMovementStarted: {
            snap = false
        }

        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            policy: ScrollBar.AlwaysOn
            minimumSize: 0.1
        }
    }
}
