import QtQuick

Item {
    id: root
    property int variant: 0
    property real radiusValue: 8
    property url sourceUrl: ""
    readonly property bool hasSource: sourceUrl.toString().length > 0

    clip: true

    GeneratedAlbumArtwork {
        anchors.fill: parent
        variant: root.variant
        radiusValue: root.radiusValue
        visible: !coverImage.visible
    }

    Rectangle {
        anchors.fill: parent
        radius: root.radiusValue
        color: "transparent"
        clip: true

        Image {
            id: coverImage
            anchors.fill: parent
            source: root.sourceUrl
            fillMode: Image.PreserveAspectCrop
            asynchronous: true
            smooth: true
            visible: root.hasSource && status !== Image.Error
        }
    }
}
