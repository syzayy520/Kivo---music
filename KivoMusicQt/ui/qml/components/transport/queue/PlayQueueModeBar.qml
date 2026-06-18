// =============================================================================
// Kivo Music - PlayQueueModeBar.qml
// 职责: 队列抽屉的「底栏」—— 播放模式胶囊(顺序/随机/单曲循环/列表循环),点击循环切换。
// 单一职责,主题感知,金色强调统一品牌。
// =============================================================================

import QtQuick
import KivoMusic

Item {
    id: bar
    // 纯展示叶子:只吃 mode(0=顺序/1=随机/2=单曲/3=列表),通过 toggleRequested 回报。
    // 控制器接线留在组合根 PlayQueueDrawer,四个叶子统一不碰全局 audioController。
    property int mode: 0
    signal toggleRequested()

    height: 64

    function modeLabel(v) {
        if (v === 1) return qsTr("Shuffle");
        if (v === 2) return qsTr("Repeat One");
        if (v === 3) return qsTr("Repeat All");
        return qsTr("In Order");
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: Theme.line
    }

    Rectangle {
        id: chip
        anchors.centerIn: parent
        width: label.implicitWidth + 40
        height: 36
        radius: 18
        color: chipMouse.containsMouse ? Theme.accentSoft : Theme.panelSoft
        border.color: Theme.line
        Behavior on color { ColorAnimation { duration: 120 } }

        Text {
            id: label
            anchors.centerIn: parent
            text: bar.modeLabel(bar.mode)
            color: Theme.accentText
            font.pixelSize: 13
            font.weight: Font.DemiBold
            font.family: Theme.fontFamily
        }

        MouseArea {
            id: chipMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: bar.toggleRequested()
        }
    }
}
