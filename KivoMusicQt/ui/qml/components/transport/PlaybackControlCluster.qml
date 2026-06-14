import QtQuick
import "../../tokens"

Column {
    id: root
    property string elapsedText: ""
    property string durationText: ""
    property real progress: 0
    property bool playing: false

    width: 300
    spacing: 3

    Theme { id: theme }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 12

        TransportButton { kind: "shuffle" }
        TransportButton { kind: "previous" }
        TransportButton { kind: root.playing ? "pause" : "play"; primary: true }
        TransportButton { kind: "next" }
        TransportButton { kind: "repeat" }
    }

    Row {
        width: parent.width
        spacing: 10

        Text {
            text: root.elapsedText
            color: theme.faint
            font.pixelSize: 10
            anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle {
            width: parent.width - 72
            height: 2
            radius: 1
            color: "#d4d8de"
            opacity: 0.86
            anchors.verticalCenter: parent.verticalCenter

            Rectangle {
                width: parent.width * Math.max(0, Math.min(1, root.progress))
                height: parent.height
                radius: 1
                color: "#24272d"
                opacity: 0.88
            }

            Rectangle {
                width: 4
                height: 4
                radius: 2
                color: "#24272d"
                opacity: 0.78
                x: parent.width * Math.max(0, Math.min(1, root.progress)) - 2
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Text {
            text: root.durationText
            color: theme.faint
            font.pixelSize: 10
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
