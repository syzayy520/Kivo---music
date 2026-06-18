import QtQuick
import "."
import KivoMusic

Column {
    id: root
    property real contentWidth: width
    property var contentModel
    readonly property int cardCount: contentModel ? contentModel.count : 0
    spacing: 12


    Text {
        text: qsTr("Continue Listening")
        color: Theme.text
        font.pixelSize: Theme.fontSectionTitle
        font.weight: Font.Bold
        font.letterSpacing: -0.2
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
