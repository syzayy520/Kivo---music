// =============================================================================
// Kivo Music - SettingsSubPage.qml
// 职责: 钻取子页的外壳——「‹ 返回」头部 + 内容插槽（用于承载 About/EULA/Privacy）
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic

Item {
    id: sub

    property string title: ""
    property StackView stack: null
    // The hosted page goes here; give it `anchors.fill: parent` (parent = holder).
    default property alias content: holder.data

    Row {
        id: header
        height: 48
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Theme.space6
        anchors.rightMargin: Theme.space6
        spacing: Theme.space3

        Rectangle {
            width: 72
            height: 32
            radius: Theme.radiusLarge
            color: backMouse.containsMouse ? Theme.panelHover : "transparent"
            anchors.verticalCenter: parent.verticalCenter
            Behavior on color { ColorAnimation { duration: Theme.animFast } }
            Text {
                anchors.centerIn: parent
                text: "‹ " + qsTr("Back")
                color: Theme.accent
                font.pixelSize: 13
                font.family: Theme.fontFamily
            }
            MouseArea {
                id: backMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: if (sub.stack) sub.stack.pop()
            }
        }
        Text {
            text: sub.title
            color: Theme.text
            font.pixelSize: 16
            font.weight: Font.DemiBold
            font.family: Theme.fontFamily
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Item {
        id: holder
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
