// =============================================================================
// Kivo Music Qt - AlbumArtwork.qml
// Album artwork — a still, pristine hero. Shows the real embedded cover if
// available, otherwise the generated fallback. (No vinyl-spin: a rotating SQUARE
// cover reads as broken; premium players keep the art still and let the ambient
// carry the "playing" energy. `playing` is kept for API compatibility.)
// =============================================================================

import QtQuick

Item {
    id: root
    property int variant: 0
    property real radiusValue: 8
    property url sourceUrl: ""
    property bool playing: false
    readonly property bool hasSource: sourceUrl.toString().length > 0 && sourceUrl.toString() !== ""
    readonly property bool coverReady: coverImage.status === Image.Ready

    clip: true

    Item {
        id: artworkContainer
        anchors.fill: parent

        // Always show generated artwork as base layer
        GeneratedAlbumArtwork {
            anchors.fill: parent
            variant: root.variant
            radiusValue: root.radiusValue
            visible: !root.coverReady
        }

        Rectangle {
            anchors.fill: parent
            radius: root.radiusValue
            color: "transparent"
            clip: true

            Image {
                id: coverImage
                anchors.fill: parent
                source: root.hasSource ? root.sourceUrl : ""
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                smooth: true
                visible: root.hasSource && status === Image.Ready
                cache: true
            }
        }
    }
}
