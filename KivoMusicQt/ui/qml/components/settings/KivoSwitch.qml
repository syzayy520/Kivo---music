// =============================================================================
// Kivo Music - KivoSwitch.qml
// 职责: 主题化开关——轨道/滑块取自 Theme 令牌（深色模式 + 强调色随之变化）。
// 用法注意: 调用方将 `checked` 绑定到模型值，并在 onToggled 里用 Qt.binding 复原
// 绑定（QtQuick Switch 点击会以命令式赋值摧毁声明式绑定）。
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic

Switch {
    id: control

    indicator: Rectangle {
        implicitWidth: 42
        implicitHeight: 24
        radius: height / 2
        color: control.checked ? Theme.accent : Theme.transportTrack
        Behavior on color { ColorAnimation { duration: Theme.animFast } }

        Rectangle {
            width: 20
            height: 20
            radius: 10
            color: Theme.npText
            anchors.verticalCenter: parent.verticalCenter
            x: control.checked ? parent.width - width - 2 : 2
            Behavior on x { NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }
        }
    }
}
