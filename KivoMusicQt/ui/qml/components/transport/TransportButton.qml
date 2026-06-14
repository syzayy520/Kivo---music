import QtQuick
import QtQuick.Controls
import "../../tokens"

Button {
    id: root
    property bool primary: false
    property string kind: "play"

    width: primary ? 34 : 26
    height: primary ? 34 : 26
    text: ""
    flat: true

    Theme { id: theme }

    background: Rectangle {
        radius: width / 2
        color: root.primary ? theme.ink : (root.hovered ? "#e9edf2" : "transparent")
        opacity: root.down ? 0.82 : 1
        border.color: root.primary ? "#2e3137" : "transparent"
        border.width: root.primary ? 1 : 0
    }

    contentItem: Canvas {
        id: glyph
        anchors.fill: parent
        onPaint: {
            const ctx = getContext("2d");
            const w = width;
            const h = height;
            const c = root.primary ? "#ffffff" : "#25282e";
            ctx.clearRect(0, 0, w, h);
            ctx.fillStyle = c;
            ctx.strokeStyle = c;
            ctx.lineWidth = root.primary ? 1.8 : 1.55;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";

            function triangle(x1, y1, x2, y2, x3, y3) {
                ctx.beginPath();
                ctx.moveTo(w * x1, h * y1);
                ctx.lineTo(w * x2, h * y2);
                ctx.lineTo(w * x3, h * y3);
                ctx.closePath();
                ctx.fill();
            }

            if (root.kind === "pause") {
                ctx.fillRect(w * 0.39, h * 0.30, w * 0.07, h * 0.40);
                ctx.fillRect(w * 0.55, h * 0.30, w * 0.07, h * 0.40);
            } else if (root.kind === "previous") {
                ctx.fillRect(w * 0.32, h * 0.30, w * 0.05, h * 0.40);
                triangle(0.70, 0.28, 0.40, 0.50, 0.70, 0.72);
            } else if (root.kind === "next") {
                ctx.fillRect(w * 0.63, h * 0.30, w * 0.05, h * 0.40);
                triangle(0.30, 0.28, 0.60, 0.50, 0.30, 0.72);
            } else if (root.kind === "shuffle") {
                ctx.beginPath();
                ctx.moveTo(w * 0.25, h * 0.35);
                ctx.lineTo(w * 0.42, h * 0.35);
                ctx.quadraticCurveTo(w * 0.55, h * 0.35, w * 0.65, h * 0.24);
                ctx.stroke();
                ctx.beginPath();
                ctx.moveTo(w * 0.25, h * 0.66);
                ctx.lineTo(w * 0.42, h * 0.66);
                ctx.quadraticCurveTo(w * 0.55, h * 0.66, w * 0.65, h * 0.76);
                ctx.stroke();
                triangle(0.66, 0.18, 0.79, 0.24, 0.66, 0.31);
                triangle(0.66, 0.69, 0.79, 0.76, 0.66, 0.82);
            } else if (root.kind === "repeat") {
                ctx.strokeRect(w * 0.28, h * 0.32, w * 0.44, h * 0.34);
                triangle(0.70, 0.24, 0.82, 0.32, 0.70, 0.40);
                triangle(0.30, 0.60, 0.18, 0.68, 0.30, 0.76);
            } else {
                triangle(0.40, 0.30, 0.40, 0.70, 0.72, 0.50);
            }
        }
    }
}
