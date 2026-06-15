import QtQuick
import QtQuick.Effects
import "../../tokens"

Row {
    id: root
    property real volume: 0

    signal volumeRequested(real newVolume)

    spacing: 7

    Theme { id: theme }

    Canvas {
        width: 18
        height: 18
        anchors.verticalCenter: parent.verticalCenter
        onPaint: {
            const ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            ctx.strokeStyle = "#3a3e45";
            ctx.fillStyle = "#3a3e45";
            ctx.lineWidth = 1.35;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";
            ctx.beginPath();
            ctx.moveTo(width * 0.18, height * 0.42);
            ctx.lineTo(width * 0.34, height * 0.42);
            ctx.lineTo(width * 0.54, height * 0.26);
            ctx.lineTo(width * 0.54, height * 0.74);
            ctx.lineTo(width * 0.34, height * 0.58);
            ctx.lineTo(width * 0.18, height * 0.58);
            ctx.closePath();
            ctx.stroke();
            ctx.beginPath();
            ctx.arc(width * 0.58, height * 0.50, width * 0.16, -0.65, 0.65);
            ctx.stroke();
        }
    }

    Rectangle {
        id: volumeTrack
        width: 52
        height: 4
        radius: 2
        color: "#e0e4e8"
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            id: volumeFill
            width: parent.width * Math.max(0, Math.min(1, root.volume))
            height: parent.height
            radius: parent.radius
            color: theme.accent

            Behavior on width {
                enabled: !volumeMouseArea.pressed
                NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
            }
        }

        Rectangle {
            id: volumeHandle
            width: 10
            height: 10
            radius: 5
            color: "#ffffff"
            border.color: theme.accent
            border.width: 2
            x: volumeFill.width - width / 2
            anchors.verticalCenter: parent.verticalCenter

            opacity: volumeMouseArea.containsMouse || volumeMouseArea.pressed ? 1.0 : 0.0
            scale: volumeMouseArea.pressed ? 1.2 : 1.0

            Behavior on opacity {
                NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
            }

            Behavior on scale {
                NumberAnimation { duration: 100; easing.type: Easing.OutBack }
            }

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#40000000"
                shadowBlur: 0.3
                shadowVerticalOffset: 2
            }
        }

        MouseArea {
            id: volumeMouseArea
            anchors.fill: parent
            anchors.margins: -6
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onPressed: function(mouse) {
                updateVolume(mouse.x);
            }

            onPositionChanged: function(mouse) {
                if (pressed) {
                    updateVolume(mouse.x);
                }
            }

            onClicked: function(mouse) {
                updateVolume(mouse.x);
            }

            function updateVolume(x) {
                var newVolume = Math.max(0, Math.min(1, x / width));
                root.volumeRequested(newVolume);
            }
        }
    }
}

