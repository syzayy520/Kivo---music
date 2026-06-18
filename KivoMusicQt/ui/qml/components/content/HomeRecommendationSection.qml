import QtQuick
import "."
import KivoMusic

Column {
    id: root
    property real contentWidth: width
    property var contentModel
    spacing: 8


    Column {
        spacing: 2

        Text {
            text: qsTr("Recommended for You")
            color: Theme.text
            font.pixelSize: Theme.fontSectionTitle
            font.weight: Font.Bold
            font.letterSpacing: -0.2
        }

        Text {
            text: qsTr("From your local library")
            color: Theme.text
            opacity: Theme.subtitleOpacity
            font.pixelSize: Theme.fontSubtitle
            font.weight: Font.Medium
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
                eyebrow: index === 0 ? qsTr("Made for You") : (model.note ?? "")
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
