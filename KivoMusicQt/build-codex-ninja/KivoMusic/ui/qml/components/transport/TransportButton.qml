import QtQuick
import QtQuick.Controls
import QtQuick.Effects
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
        color: root.primary ? theme.ink : (root.hovered ? "#f0f2f5" : "transparent")
        opacity: root.down ? 0.82 : 1
        border.color: root.primary ? "#2e3137" : "transparent"
        border.width: root.primary ? 1 : 0

        // 添加平滑过渡
        Behavior on color {
            ColorAnimation { duration: 150; easing.type: Easing.OutQuad }
        }

        Behavior on opacity {
            NumberAnimation { duration: 100 }
        }

        // 主按钮阴影
        layer.enabled: root.primary
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#40000000"
            shadowBlur: 0.4
            shadowVerticalOffset: 3
        }
    }

    contentItem: Canvas {
        id: glyph
        anchors.fill: parent

        // 添加平滑的图标变化动画
        property real morphProgress: root.kind === "pause" ? 1.0 : 0.0

        Behavior on morphProgress {
            NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
        }

        onMorphProgressChanged: requestPaint()

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

            // 播放/暂停变形动画
            if (root.kind === "pause" || root.kind === "play") {
                const morph = morphProgress;

                // 左侧形状：从三角形左边到暂停左条
                const leftX1 = 0.40 - morph * 0.01;
                const leftX2 = 0.39 + morph * 0.00;
                const leftWidth = 0.07;

                ctx.fillRect(w * leftX1, h * 0.30, w * leftWidth, h * 0.40);

                // 右侧形状：从三角形尖端到暂停右条
                if (morph < 0.5) {
                    // 三角形变窄
                    triangle(0.40, 0.30, 0.40, 0.70, 0.72 - morph * 0.4, 0.50);
                } else {
                    // 变成右条
                    ctx.fillRect(w * 0.55, h * 0.30, w * 0.07, h * 0.40);
                }
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
            }
        }
    }
}
