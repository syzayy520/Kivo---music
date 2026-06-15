// =============================================================================
// Kivo Music Qt - AlbumArtwork.qml
// Album artwork with rotation animation during playback.
// Shows real cover if available, otherwise generated colorful artwork.
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

    // Rotation animation
    RotationAnimation {
        id: spinAnimation
        target: artworkContainer
        property: "rotation"
        from: 0
        to: 360
        duration: 20000
        loops: Animation.Infinite
        running: root.playing
        easing.type: Easing.Linear
        onRunningChanged: {
            if (!running) artworkContainer.rotation = 0
        }
    }

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
