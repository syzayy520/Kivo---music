import QtQuick
import "."
import "../../tokens"

Column {
    id: root
    property real contentWidth: width
    property var contentModel
    readonly property int cardCount: contentModel ? contentModel.count : 0
    spacing: 12

    Theme { id: theme }

    Text {
        text: "Pick Up Listening"
        color: theme.text
        font.pixelSize: 18
        font.weight: Font.DemiBold
    }

    Row {
        id: cardRow
        width: root.contentWidth
        spacing: 18

        Repeater {
            model: root.contentModel

            ContinueListeningCard {
                width: root.cardCount > 0
                    ? (cardRow.width - cardRow.spacing * (root.cardCount - 1))
                        / root.cardCount
                    : 0
                title: model.title
                artist: model.subtitle
                context: model.note
                artVariant: model.artVariant
                coverUrl: model.coverUrl
                filePath: model.filePath
                onPlayRequested: function(path) {
                    audioController.openFile(path)
                }
            }
        }
    }
}
