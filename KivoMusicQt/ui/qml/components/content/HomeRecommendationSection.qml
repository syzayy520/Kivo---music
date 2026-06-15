import QtQuick
import "."
import "../../tokens"

Column {
    id: root
    property real contentWidth: width
    property var contentModel
    spacing: 8

    Theme { id: theme }

    Column {
        spacing: 1

        Text {
            text: "Top Picks for You"
            color: theme.text
            font.pixelSize: 18
            font.weight: Font.DemiBold
        }

        Text {
            text: "Curated from your local library"
            color: theme.muted
            font.pixelSize: 13
        }
    }

    Item {
        id: shelf
        width: root.contentWidth
        height: cardHeight
        clip: true

        readonly property real cardWidth: Math.max(168, Math.min(204, (width - 54) / 4.6))
        readonly property real cardHeight: Math.round(cardWidth * 1.22) + 22

        ListView {
            anchors.fill: parent
            model: root.contentModel
            orientation: ListView.Horizontal
            spacing: 18
            boundsBehavior: Flickable.StopAtBounds
            flickDeceleration: 2800
            maximumFlickVelocity: 2400
            snapMode: ListView.SnapToItem
            clip: true

            delegate: TopPickCard {
                width: shelf.cardWidth
                eyebrow: index === 0 ? "Made for You" : model.note
                title: model.title
                subtitle: model.subtitle
                artVariant: model.artVariant
                coverUrl: model.coverUrl
                filePath: model.filePath
                editorial: index === 0 || index === 3
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
