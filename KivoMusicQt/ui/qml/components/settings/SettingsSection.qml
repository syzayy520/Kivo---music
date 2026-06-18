// =============================================================================
// Kivo Music - SettingsSection.qml
// 职责: 设置页内的「带标题分组」容器（替代散落的大写标题 Text）
// =============================================================================

import QtQuick
import KivoMusic

Column {
    id: section

    property string title: ""
    // Children placed inside <SettingsSection> land in the inner content column.
    default property alias content: contentColumn.data

    width: parent ? parent.width : 0
    spacing: Theme.space3

    Text {
        visible: section.title.length > 0
        text: section.title
        color: Theme.muted
        font.pixelSize: 11
        font.weight: Font.DemiBold
        font.capitalization: Font.AllUppercase
        font.family: Theme.fontFamily
    }

    Column {
        id: contentColumn
        width: parent.width
        spacing: Theme.space2
    }
}
