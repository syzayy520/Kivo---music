// =============================================================================
// Kivo Music - NowPlayingMeta.qml
// 职责: 沉浸视图的标题 / 艺人文字块。单一职责。
// =============================================================================

import QtQuick
import KivoMusic

Column {
    id: root
    property string title: ""
    property string artist: ""
    spacing: 6

    Text {
        width: parent.width
        text: root.title
        color: Theme.npText
        font.pixelSize: 26
        font.weight: Font.Bold
        font.family: Theme.fontFamily
        elide: Text.ElideRight
        horizontalAlignment: Text.AlignHCenter
    }
    Text {
        width: parent.width
        text: root.artist
        color: Theme.npTextDim
        font.pixelSize: 16
        font.family: Theme.fontFamily
        elide: Text.ElideRight
        horizontalAlignment: Text.AlignHCenter
    }
}
