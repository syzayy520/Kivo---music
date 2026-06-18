// =============================================================================
// Kivo Music Qt - CustomScrollBar.qml
// 自定义滚动条样式
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic

ScrollBar {
    id: control


    implicitWidth: 8
    implicitHeight: 8

    contentItem: Rectangle {
        implicitWidth: 6
        implicitHeight: 6
        radius: 3

        color: control.pressed ? Theme.muted : (control.hovered ? Theme.faint : Theme.inactive)
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
