// =============================================================================
// Kivo Music - SettingsToggleRow.qml
// 职责: 设置行——图标 + 标题(+副标题) + 开关
// =============================================================================

import QtQuick
import KivoMusic

Rectangle {
    id: row

    property string icon: ""
    property string label: ""
    property string sublabel: ""
    property bool checked: false
    signal toggled(bool value)

    width: parent ? parent.width : 0
    height: 56
    radius: Theme.radiusXL
    color: Theme.panel
    border.color: Theme.line

    Text {
        id: glyph
        visible: row.icon.length > 0
        text: row.icon
        font.pixelSize: 20
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.verticalCenter: parent.verticalCenter
    }

    KivoSwitch {
        id: sw
        checked: row.checked
        text: row.label
        anchors.right: parent.right
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        // Clicking a Switch imperatively assigns `checked`, destroying the
        // `checked: row.checked` binding. Emit intent, then re-establish the
        // binding so the control keeps tracking row.checked (e.g. Theme.isDark
        // flipped from the TopBar toggle).
        onToggled: {
            row.toggled(checked)
            sw.checked = Qt.binding(function() { return row.checked })
        }
    }

    Column {
        anchors.left: glyph.visible ? glyph.right : parent.left
        anchors.leftMargin: glyph.visible ? 12 : 16
        anchors.right: sw.left
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2

        Text {
            text: row.label
            color: Theme.text
            font.pixelSize: 14
            font.family: Theme.fontFamily
            elide: Text.ElideRight
            width: parent.width
        }
        Text {
            visible: row.sublabel.length > 0
            text: row.sublabel
            color: Theme.faint
            font.pixelSize: 11
            font.family: Theme.fontFamily
            elide: Text.ElideRight
            width: parent.width
        }
    }
}
