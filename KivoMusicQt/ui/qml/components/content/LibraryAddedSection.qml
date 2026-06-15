import QtQuick
import "."
import "../../tokens"

Column {
    id: root
    property real contentWidth: width
    property var contentModel
    spacing: 14

    Theme { id: theme }

    Row {
        width: root.contentWidth

        Text {
            text: "Recently Added"
            color: theme.text
            font.pixelSize: 18
            font.weight: Font.DemiBold
        }

        Item {
            width: Math.max(12, parent.width - 186)
            height: 1
        }

        Text {
            text: "See All >"
            color: theme.muted
            font.pixelSize: 13
        }
    }

    Item {
        id: shelf
        width: root.contentWidth
        height: tileWidth + 58
        clip: true

        readonly property real tileWidth: Math.max(126, Math.min(148, (width - 110) / 6.7))

        ListView {
            anchors.fill: parent
            model: root.contentModel
            orientation: ListView.Horizontal
            spacing: 22
            boundsBehavior: Flickable.StopAtBounds
            flickDeceleration: 2800
            maximumFlickVelocity: 2400
            snapMode: ListView.SnapToItem
            clip: true

            delegate: AlbumTile {
                width: shelf.tileWidth
                title: model.title
                subtitle: model.subtitle
                note: model.note
                artVariant: model.artVariant
                coverUrl: model.coverUrl
                filePath: model.filePath
                onPlayRequested: function(path) {
                    audioController.openFile(path)
                }
            }
        }

        ShelfEdgeFade {
            height: parent.height
            anchors.right: parent.right
        }
    }
}
