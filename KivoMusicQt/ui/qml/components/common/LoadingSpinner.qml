// =============================================================================
// Kivo Music - LoadingSpinner.qml
// 职责: 通用加载指示器 —— 旋转的圆环，主题感知颜色。单一职责：只画spinner。
// =============================================================================

import QtQuick
import KivoMusic

Item {
    id: root
    property color color: Theme.accent
    property real lineWidth: 2

    width: 24
    height: 24

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            const centerX = width / 2
            const centerY = height / 2
            const radius = Math.min(width, height) / 2 - root.lineWidth

            // Draw arc (3/4 circle)
            ctx.strokeStyle = root.color
            ctx.lineWidth = root.lineWidth
            ctx.lineCap = "round"
            ctx.beginPath()
            ctx.arc(centerX, centerY, radius, 0, Math.PI * 1.5)
            ctx.stroke()
        }

        // Continuous rotation animation
        RotationAnimator on rotation {
            from: 0
            to: 360
            duration: 1000
            loops: Animation.Infinite
            running: root.visible
        }
    }

    // Repaint when color changes
    Connections {
        target: root
        function onColorChanged() { canvas.requestPaint() }
    }

    Component.onCompleted: canvas.requestPaint()
}
