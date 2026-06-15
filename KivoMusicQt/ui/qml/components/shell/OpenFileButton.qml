import QtQuick
import QtQuick.Effects
import "../../tokens"

Rectangle {
    id: root
    signal triggered()

    width: 34
    height: 34
    radius: 17
    color: mouseArea.containsMouse ? "#ffffff" : "#f5f6f8"
    border.color: mouseArea.containsMouse ? "#d7dbe1" : "#e4e7ec"
    border.width: 1
    scale: mouseArea.pressed ? 0.96 : 1.0
    opacity: mouseArea.pressed ? 0.9 : 1.0

    Theme { id: theme }

    Behavior on color {
        ColorAnimation { duration: 120; easing.type: Easing.OutQuad }
    }

    Behavior on border.color {
        ColorAnimation { duration: 120; easing.type: Easing.OutQuad }
    }

    Behavior on scale {
        NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
    }

    Behavior on opacity {
        NumberAnimation { duration: 100 }
    }

    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#20000000"
        shadowBlur: mouseArea.containsMouse ? 0.42 : 0.24
        shadowVerticalOffset: mouseArea.containsMouse ? 4 : 2

        Behavior on shadowBlur {
            NumberAnimation { duration: 150 }
        }

        Behavior on shadowVerticalOffset {
            NumberAnimation { duration: 150 }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true
        onClicked: root.triggered()
    }

    Canvas {
        anchors.centerIn: parent
        width: 18
        height: 18

        onPaint: {
            const ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            ctx.strokeStyle = theme.ink;
            ctx.lineWidth = 1.45;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";

            ctx.beginPath();
            ctx.moveTo(width * 0.18, height * 0.42);
            ctx.lineTo(width * 0.18, height * 0.74);
            ctx.quadraticCurveTo(width * 0.18, height * 0.84, width * 0.28, height * 0.84);
            ctx.lineTo(width * 0.76, height * 0.84);
            ctx.quadraticCurveTo(width * 0.86, height * 0.84, width * 0.86, height * 0.74);
            ctx.lineTo(width * 0.86, height * 0.34);
            ctx.quadraticCurveTo(width * 0.86, height * 0.25, width * 0.76, height * 0.25);
            ctx.lineTo(width * 0.52, height * 0.25);
            ctx.lineTo(width * 0.45, height * 0.16);
            ctx.lineTo(width * 0.26, height * 0.16);
            ctx.quadraticCurveTo(width * 0.18, height * 0.16, width * 0.18, height * 0.25);
            ctx.closePath();
            ctx.stroke();

            ctx.beginPath();
            ctx.moveTo(width * 0.52, height * 0.38);
            ctx.lineTo(width * 0.52, height * 0.68);
            ctx.moveTo(width * 0.40, height * 0.56);
            ctx.lineTo(width * 0.52, height * 0.68);
            ctx.lineTo(width * 0.64, height * 0.56);
            ctx.stroke();
        }
    }
}
