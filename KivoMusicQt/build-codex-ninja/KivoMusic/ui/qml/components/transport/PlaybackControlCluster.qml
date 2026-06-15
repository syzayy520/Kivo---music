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
    spacing: 3

    Theme { id: theme }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 12

        TransportButton {
            kind: "shuffle"
            // TODO: connect to shuffle function
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
            // TODO: connect to repeat function
        }
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

        // 进度条容器
        Rectangle {
            id: progressTrack
            width: parent.width - 72
            height: 4
            radius: 2
            color: "#e0e4e8"
            anchors.verticalCenter: parent.verticalCenter

            // 已播放部分
            Rectangle {
                id: progressFill
                width: parent.width * Math.max(0, Math.min(1, root.progress))
                height: parent.height
                radius: parent.radius
                color: theme.accent

                Behavior on width {
                    enabled: !progressMouseArea.pressed
                    NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
                }
            }

            // 拖动把手
            Rectangle {
                id: progressHandle
                width: 12
                height: 12
                radius: 6
                color: "#ffffff"
                border.color: theme.accent
                border.width: 2
                x: progressFill.width - width / 2
                anchors.verticalCenter: parent.verticalCenter

                opacity: progressMouseArea.containsMouse || progressMouseArea.pressed ? 1.0 : 0.0
                scale: progressMouseArea.pressed ? 1.2 : 1.0

                Behavior on opacity {
                    NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
                }

                Behavior on scale {
                    NumberAnimation { duration: 100; easing.type: Easing.OutBack }
                }

                // 把手阴影
                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: "#40000000"
                    shadowBlur: 0.3
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
                    var pos = Math.max(0, Math.min(1, mouse.x / width));
                    root.seekRequested(pos);
                }
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
