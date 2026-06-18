// =============================================================================
// Kivo Music - windows/mini/MiniHeader.qml
// 职责: 迷你播放器顶部栏 —— 品牌文字 + 关闭按钮。纯展示+单交互。
// =============================================================================

import QtQuick
import KivoMusic

Item {
    id: root
    implicitHeight: 32

    signal closeRequested()

    Text {
        anchors.centerIn: parent
        text: qsTr("Kivo Music")
        color: Theme.muted
        font.pixelSize: 11
        font.weight: Font.Medium
    }

    Rectangle {
        width: 28; height: 28
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        radius: 14
        color: closeMa.containsMouse ? Theme.panelSoft : "transparent"
        Behavior on color { ColorAnimation { duration: 120 } }

        Text {
            anchors.centerIn: parent
            text: "×"
            font.pixelSize: 16
            color: Theme.muted
        }

        MouseArea {
            id: closeMa
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.closeRequested()
        }
    }
}
