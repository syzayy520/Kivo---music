// =============================================================================
// Kivo Music - windows/mini/MiniTrackInfo.qml
// 职责: 迷你播放器曲名/艺术家文字展示。纯展示，无交互。
// =============================================================================

import QtQuick
import KivoMusic

Column {
    id: root
    spacing: 2

    property string title: ""
    property string artist: ""

    Text {
        width: parent.width
        text: root.title.length > 0 ? root.title : qsTr("Not Playing")
        color: Theme.text
        font.pixelSize: 15
        font.weight: Font.DemiBold
        elide: Text.ElideRight
        maximumLineCount: 1
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        width: parent.width
        text: root.artist
        color: Theme.muted
        font.pixelSize: 12
        elide: Text.ElideRight
        maximumLineCount: 1
        horizontalAlignment: Text.AlignHCenter
        visible: root.artist.length > 0
    }
}
