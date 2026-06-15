import QtQuick
import QtQuick.Effects
import "../../tokens"

Column {
    id: root
    property string elapsedText: ""
    property string durationText: ""
    property real progress: 0
    property bool playing: false

    signal playPauseClicked()
    signal previousClicked()
    signal nextClicked()
    signal seekRequested(real position)

    width: 300
    spacing: 4

    Theme { id: theme }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 12

        TransportButton {
            kind: "shuffle"
        }

        TransportButton {
            kind: "previous"
            onClicked: root.previousClicked()
        }

        TransportButton {
            kind: root.playing ? "pause" : "play"
            primary: true
            onClicked: root.playPauseClicked()
        }

        TransportButton {
            kind: "next"
            onClicked: root.nextClicked()
        }

        TransportButton {
            kind: "repeat"
        }
    }

    Row {
        width: parent.width
        height: 14
        spacing: 9

        Text {
            width: 30
            text: root.elapsedText
            color: theme.faint
            font.pixelSize: 10
            horizontalAlignment: Text.AlignRight
            anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle {
            id: progressTrack
            width: parent.width - 78
            height: progressMouseArea.containsMouse || progressMouseArea.pressed ? 4 : 3
            radius: height / 2
            color: theme.transportTrack
            anchors.verticalCenter: parent.verticalCenter

            Behavior on height {
                NumberAnimation { duration: 120; easing.type: Easing.OutQuad }
            }

            Rectangle {
                id: progressFill
                width: parent.width * Math.max(0, Math.min(1, root.progress))
                height: parent.height
                radius: parent.radius
                color: theme.transportFill

                Behavior on width {
                    enabled: !progressMouseArea.pressed
                    NumberAnimation { duration: 140; easing.type: Easing.OutQuad }
                }
            }

            Rectangle {
                id: progressHandle
                width: 10
                height: 10
                radius: 5
                color: "#ffffff"
                border.color: theme.transportFill
                border.width: 1
                x: Math.max(-width / 2, Math.min(progressFill.width - width / 2, progressTrack.width - width / 2))
                anchors.verticalCenter: parent.verticalCenter
                opacity: progressMouseArea.containsMouse || progressMouseArea.pressed ? 1.0 : 0.0
                scale: progressMouseArea.pressed ? 1.2 : 1.0

                Behavior on opacity {
                    NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
                }

                Behavior on scale {
                    NumberAnimation { duration: 100; easing.type: Easing.OutBack }
                }

                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: "#26000000"
                    shadowBlur: 0.25
                    shadowVerticalOffset: 2
                }
            }

            MouseArea {
                id: progressMouseArea
                anchors.fill: parent
                anchors.margins: -8
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                property real pressedProgress: 0

                onPressed: function(mouse) {
                    pressedProgress = Math.max(0, Math.min(1, mouse.x / width));
                }

                onPositionChanged: function(mouse) {
                    if (pressed) {
                        pressedProgress = Math.max(0, Math.min(1, mouse.x / width));
                    }
                }

                onReleased: {
                    root.seekRequested(pressedProgress);
                }

                onClicked: function(mouse) {
                    const position = Math.max(0, Math.min(1, mouse.x / width));
                    root.seekRequested(position);
                }
            }
        }

        Text {
            width: 30
            text: root.durationText
            color: theme.faint
            font.pixelSize: 10
            horizontalAlignment: Text.AlignLeft
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
