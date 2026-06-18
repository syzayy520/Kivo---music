// =============================================================================
// Kivo Music - SettingsEntityListRow.qml
// 职责: 通用「实体行」——图标 + 标题 + 副标题(路径) + 可选开关 + 可选移除
// 复用: 音乐来源、输出设备、云后端、AI 提供方等都用同一行
// =============================================================================

import QtQuick
import KivoMusic

Rectangle {
    id: row

    property string icon: ""
    property string title: ""
    property string subtitle: ""
    property bool showToggle: false
    property bool toggleChecked: false
    property bool showRemove: false
    signal toggled(bool value)
    signal removeClicked()

    width: parent ? parent.width : 0
    height: 60
    radius: Theme.radiusXL
    color: Theme.panel
    border.color: Theme.line

    Text {
        id: glyph
        text: row.icon
        font.pixelSize: 20
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.verticalCenter: parent.verticalCenter
    }

    // Remove (far right).
    Rectangle {
        id: removeBtn
        visible: row.showRemove
        width: 28
        height: 28
        radius: 14
        color: removeMouse.containsMouse ? Theme.accentHot : "transparent"
        anchors.right: parent.right
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter

        Behavior on color { ColorAnimation { duration: Theme.animFast } }

        Text {
            anchors.centerIn: parent
            text: "✕"
            color: removeMouse.containsMouse ? "white" : Theme.faint
            font.pixelSize: 14
        }
        MouseArea {
            id: removeMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: row.removeClicked()
        }
    }

    // Toggle (left of remove).
    KivoSwitch {
        id: enabledSwitch
        visible: row.showToggle
        checked: row.toggleChecked
        anchors.right: row.showRemove ? removeBtn.left : parent.right
        anchors.rightMargin: row.showRemove ? 0 : 12
        anchors.verticalCenter: parent.verticalCenter
        // Re-establish the binding the click destroys, so the toggle keeps
        // reflecting the model value (e.g. after an external enable/disable).
        onToggled: {
            row.toggled(checked)
            enabledSwitch.checked = Qt.binding(function() { return row.toggleChecked })
        }
    }

    Column {
        anchors.left: glyph.right
        anchors.leftMargin: 12
        anchors.right: enabledSwitch.visible
            ? enabledSwitch.left
            : (removeBtn.visible ? removeBtn.left : parent.right)
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2

        Text {
            text: row.title
            color: Theme.text
            font.pixelSize: 14
            font.family: Theme.fontFamily
            elide: Text.ElideRight
            width: parent.width
        }
        Text {
            visible: row.subtitle.length > 0
            text: row.subtitle
            color: Theme.faint
            font.pixelSize: 11
            font.family: Theme.fontFamily
            elide: Text.ElideMiddle
            width: parent.width
        }
    }
}
