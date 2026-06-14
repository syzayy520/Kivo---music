import QtQuick
import "../../tokens"

Row {
    id: root
    property real volume: 0

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
        width: 52
        height: 2
        radius: 1
        color: "#d4d8de"
        opacity: 0.88
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            width: parent.width * Math.max(0, Math.min(1, root.volume))
            height: parent.height
            radius: 1
            color: "#24272d"
            opacity: 0.86
        }
    }
}
