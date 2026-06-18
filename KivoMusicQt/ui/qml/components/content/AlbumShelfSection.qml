import QtQuick
import "."
import KivoMusic

Column {
    id: root
    property string title: ""
    property string actionText: qsTr("All ›")
    property real contentWidth: width
    property var contentModel
    readonly property int cardCount: contentModel ? contentModel.count : 0
    spacing: 14
    visible: cardCount > 0


    Item {
        width: root.contentWidth
        height: 24

        Text {
            id: titleText
            text: root.title
            color: Theme.text
            font.pixelSize: Theme.fontSectionTitle
            font.weight: Font.Bold
            font.letterSpacing: -0.2
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: action
            text: root.actionText
            color: Theme.text
            opacity: Theme.subtitleOpacity
            font.pixelSize: Theme.fontSubtitle
            font.weight: Font.Medium
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
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
