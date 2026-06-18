import QtQuick
import "../artwork"
import KivoMusic

Item {
    id: root
    property string title: ""
    property string subtitle: ""
    property string eyebrow: ""
    property int artVariant: 0
    property url coverUrl: ""
    property string filePath: ""
    signal playRequested(string filePath)

    height: width + 66


    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            if (root.filePath.length > 0)
                root.playRequested(root.filePath)
        }
    }

    Column {
        anchors.fill: parent
        spacing: 7

        AlbumArtwork {
            width: parent.width
            height: parent.width
            radiusValue: 8
            variant: root.artVariant
            sourceUrl: root.coverUrl
        }

        Text {
            width: parent.width
            text: root.eyebrow
            color: Theme.muted
            font.pixelSize: 11
            font.weight: Font.DemiBold
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.title
            color: Theme.text
            font.pixelSize: 14
            font.weight: Font.DemiBold
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.subtitle
            color: Theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }
}
