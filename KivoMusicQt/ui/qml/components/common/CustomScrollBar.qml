// =============================================================================
// Kivo Music Qt - CustomScrollBar.qml
// 自定义滚动条样式
// =============================================================================

import QtQuick
import QtQuick.Controls
import "../../tokens"

ScrollBar {
    id: control

    Theme { id: theme }

    implicitWidth: 8
    implicitHeight: 8

    contentItem: Rectangle {
        implicitWidth: 6
        implicitHeight: 6
        radius: 3

        color: control.pressed ? theme.muted : (control.hovered ? "#a0a4ab" : "#c0c4cb")
        opacity: control.active || control.hovered ? 1.0 : 0.4

        Behavior on color {
            ColorAnimation { duration: 150 }
        }

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    background: Rectangle {
        color: "transparent"
        opacity: 0
    }
}
