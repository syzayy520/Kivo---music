// =============================================================================
// Kivo Music - PlayQueueHeader.qml
// 职责: 队列抽屉的「头部」—— 标题「播放队列」+ 数量 + 关闭按钮。单一职责,主题感知。
// =============================================================================

import QtQuick
import KivoMusic

Item {
    id: header
    property int count: 0
    signal closeRequested()

    height: 64

    Column {
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2

        Text {
            text: qsTr("Play Queue")
            color: Theme.text
            font.pixelSize: 17
            font.weight: Font.DemiBold
            font.family: Theme.fontFamily
        }
        Text {
            text: header.count > 0 ? qsTr("%1 tracks").arg(header.count) : qsTr("Empty")
            color: Theme.muted
            font.pixelSize: 12
            font.family: Theme.fontFamily
        }
    }

    Rectangle {
        id: closeBtn
        width: 30
        height: 30
        radius: 15
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        color: closeMouse.containsMouse ? Theme.panelHover : "transparent"
        Behavior on color { ColorAnimation { duration: 120 } }

        Text {
            anchors.centerIn: parent
            text: "✕"
            color: Theme.muted
            font.pixelSize: 14
        }
        MouseArea {
            id: closeMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: header.closeRequested()
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: Theme.line
    }
}
