// =============================================================================
// Kivo Music - SettingsValueRow.qml
// 职责: 只读取值行——图标 + 标题 + 右侧取值（可标记为「未启用/受限」）
// =============================================================================

import QtQuick
import KivoMusic

Rectangle {
    id: row

    property string icon: ""
    property string label: ""
    property string value: ""
    // When false, the row reads as gated/unavailable (e.g. a backend not yet wired).
    property bool available: true
    property string note: ""  // optional inline reason shown under the label when gated

    width: parent ? parent.width : 0
    height: row.note.length > 0 && !row.available ? 64 : 56
    radius: Theme.radiusXL
    color: Theme.panel
    border.color: Theme.line
    opacity: row.available ? 1.0 : 0.65

    Text {
        id: glyph
        visible: row.icon.length > 0
        text: row.icon
        font.pixelSize: 20
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        id: valueText
        text: row.value
        color: Theme.faint
        font.pixelSize: 13
        font.family: Theme.fontFamily
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
    }

    Column {
        anchors.left: glyph.visible ? glyph.right : parent.left
        anchors.leftMargin: glyph.visible ? 12 : 16
        anchors.right: valueText.left
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
            visible: row.note.length > 0 && !row.available
            text: row.note
            color: Theme.faint
            font.pixelSize: 11
            font.family: Theme.fontFamily
            elide: Text.ElideRight
            width: parent.width
        }
    }
}
