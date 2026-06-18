// =============================================================================
// Kivo Music - NowPlayingArtwork.qml
// 职责: 沉浸视图的大封面 + 阴影。单一职责，只吃一个 source。
// =============================================================================

import QtQuick
import QtQuick.Effects
import "../artwork"
import KivoMusic

Item {
    id: root
    property url source: ""
    property int size: 340
    property bool playing: false

    implicitWidth: size
    implicitHeight: size

    AlbumArtwork {
        anchors.fill: parent
        radiusValue: 16
        sourceUrl: root.source
        playing: root.playing

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Theme.npScrimHeavy
            shadowBlur: 1.0
            shadowVerticalOffset: 26
        }
    }
}
