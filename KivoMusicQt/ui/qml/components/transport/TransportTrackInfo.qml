// =============================================================================
// Kivo Music Qt - TransportTrackInfo.qml
// Track info display with real album artwork in transport bar.
// =============================================================================

import QtQuick
import "../artwork"
import KivoMusic

Item {
    id: root
    property string title: ""
    property string subtitle: ""
    property url coverUrl: ""
    property bool playing: false

    height: 48


    AlbumArtwork {
        id: art
        width: 44
        height: 44
        radiusValue: 8
        sourceUrl: root.coverUrl
        variant: audioController.coverVariantSeed
        playing: root.playing
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
    }

    Column {
        anchors.left: art.right
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 3

        Text {
            width: parent.width
            // 占位文案在表现层本地化:后端无曲目时给空串。
            text: root.title.length > 0 ? root.title : qsTr("Not Playing")
            color: Theme.text
            font.pixelSize: 12
            font.weight: Font.Medium
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.subtitle.length > 0 ? root.subtitle : qsTr("Select a song to start playing")
            color: Theme.muted
            font.pixelSize: 10
            elide: Text.ElideRight
        }
    }
}
