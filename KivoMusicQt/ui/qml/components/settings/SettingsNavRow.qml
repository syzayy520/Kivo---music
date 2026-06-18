// =============================================================================
// Kivo Music - SettingsNavRow.qml
// 职责: 可点击的导航行——图标 + 标题 + 右尖括号（用于钻取/外链）
// =============================================================================

import QtQuick
import KivoMusic

Rectangle {
    id: row

    property string icon: ""
    property string label: ""
    signal clicked()

    width: parent ? parent.width : 0
    height: 48
    radius: Theme.radiusXL
    color: hover.containsMouse ? Theme.panelHover : Theme.panel
    border.color: Theme.line

    Behavior on color { ColorAnimation { duration: Theme.animFast } }

    Text {
        id: glyph
        visible: row.icon.length > 0
        text: row.icon
        font.pixelSize: 18
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        id: chevron
        text: "›"
        color: Theme.faint
        font.pixelSize: 18
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        text: row.label
        color: Theme.text
        font.pixelSize: 14
        font.family: Theme.fontFamily
        elide: Text.ElideRight
        anchors.left: glyph.visible ? glyph.right : parent.left
        anchors.leftMargin: glyph.visible ? 12 : 16
        anchors.right: chevron.left
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        id: hover
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        Accessible.role: Accessible.Button
        Accessible.name: row.label
        Accessible.onPressAction: row.clicked()
        onClicked: row.clicked()
    }
}
