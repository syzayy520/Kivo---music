// =============================================================================
// Kivo Music - TransportButton.qml
// Apple Music-level transport button with refined glyphs and haptic-like feedback
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import "../../tokens"

Button {
    id: root
    property bool primary: false
    property string kind: "play"

    width: primary ? 40 : 30
    height: primary ? 40 : 30
    text: ""
    flat: true

    Theme { id: theme }

    background: Rectangle {
        radius: width / 2
        color: {
            if (root.primary) return theme.text;
            if (root.hovered) return "#f0f1f3";
            return "transparent";
        }
        opacity: root.down ? 0.85 : 1
        border.color: root.primary ? "#2c2e33" : "transparent"
        border.width: root.primary ? 1 : 0

        Behavior on color {
            ColorAnimation { duration: 120; easing.type: Easing.OutQuad }
        }
        Behavior on opacity {
            NumberAnimation { duration: 80 }
        }

        // Enhanced shadow for primary button
        layer.enabled: root.primary
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#45000000"
            shadowBlur: 0.45
            shadowVerticalOffset: 3
        }
    }

    contentItem: Canvas {
        id: glyph
        anchors.fill: parent

        property real morphProgress: root.kind === "pause" ? 1.0 : 0.0

        Behavior on morphProgress {
            NumberAnimation { duration: 180; easing.type: Easing.InOutCubic }
        }

        onMorphProgressChanged: requestPaint()

        onPaint: {
            const ctx = getContext("2d");
            const w = width;
            const h = height;
            const c = root.primary ? "#ffffff" : "#1d1d1f";
            ctx.clearRect(0, 0, w, h);
            ctx.fillStyle = c;
            ctx.strokeStyle = c;
            ctx.lineWidth = root.primary ? 1.8 : 1.5;
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

            if (root.kind === "play" || root.kind === "pause") {
                const m = morphProgress;
                const lx = 0.39 - m * 0.01;
                const lw = 0.08;
                ctx.fillRect(w * lx, h * 0.29, w * lw, h * 0.42);

                if (m < 0.5) {
                    triangle(0.40, 0.29, 0.40, 0.71, 0.74 - m * 0.4, 0.50);
                } else {
                    ctx.fillRect(w * 0.56, h * 0.29, w * 0.08, h * 0.42);
                }
            } else if (root.kind === "previous") {
                ctx.fillRect(w * 0.30, h * 0.29, w * 0.06, h * 0.42);
                triangle(0.70, 0.27, 0.38, 0.50, 0.70, 0.73);
            } else if (root.kind === "next") {
                ctx.fillRect(w * 0.64, h * 0.29, w * 0.06, h * 0.42);
                triangle(0.30, 0.27, 0.62, 0.50, 0.30, 0.73);
            } else if (root.kind === "shuffle") {
                ctx.lineWidth = 1.6;
                ctx.beginPath();
                ctx.moveTo(w * 0.24, h * 0.34);
                ctx.lineTo(w * 0.42, h * 0.34);
                ctx.quadraticCurveTo(w * 0.56, h * 0.34, w * 0.66, h * 0.22);
                ctx.stroke();
                ctx.beginPath();
                ctx.moveTo(w * 0.24, h * 0.66);
                ctx.lineTo(w * 0.42, h * 0.66);
                ctx.quadraticCurveTo(w * 0.56, h * 0.66, w * 0.66, h * 0.78);
                ctx.stroke();
                triangle(0.67, 0.16, 0.82, 0.22, 0.67, 0.29);
                triangle(0.67, 0.71, 0.82, 0.78, 0.67, 0.84);
            } else if (root.kind === "repeat") {
                ctx.lineWidth = 1.6;
                ctx.strokeRect(w * 0.27, h * 0.31, w * 0.46, h * 0.35);
                triangle(0.71, 0.23, 0.84, 0.31, 0.71, 0.39);
                triangle(0.29, 0.61, 0.16, 0.69, 0.29, 0.77);
            } else if (root.kind === "queue") {
                ctx.lineWidth = 1.6;
                for (let i = 0; i < 3; i++) {
                    ctx.fillRect(w * 0.25, h * (0.22 + i * 0.26), w * 0.5, h * 0.06);
                    ctx.fillRect(w * 0.25, h * (0.22 + i * 0.26), w * 0.12, h * 0.06);
                }
            } else if (root.kind === "lyrics") {
                ctx.lineWidth = 1.5;
                for (let i = 0; i < 3; i++) {
                    const y = 0.28 + i * 0.22;
                    ctx.beginPath();
                    ctx.moveTo(w * 0.22, h * y);
                    ctx.lineTo(w * 0.78, h * y);
                    ctx.stroke();
                }
                ctx.fillRect(w * 0.22, h * 0.22, w * 0.45, h * 0.08);
            }
        }
    }
}
