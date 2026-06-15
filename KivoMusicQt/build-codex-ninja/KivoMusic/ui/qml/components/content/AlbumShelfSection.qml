import QtQuick
import "."
import "../../tokens"

Column {
    id: root
    property string title: ""
    property string actionText: "See All >"
    property real contentWidth: width
    property var contentModel
    readonly property int cardCount: contentModel ? contentModel.count : 0
    spacing: 14
    visible: cardCount > 0

    Theme { id: theme }

    Row {
        width: root.contentWidth

        Text {
            id: titleText
            text: root.title
            color: theme.text
            font.pixelSize: 18
            font.weight: Font.DemiBold
        }

        Item {
            width: Math.max(12, parent.width - titleText.width - action.width - 24)
            height: 1
        }

        Text {
            id: action
            text: root.actionText
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
            }
        }

        ShelfEdgeFade {
            height: parent.height
            anchors.right: parent.right
        }
    }
}
