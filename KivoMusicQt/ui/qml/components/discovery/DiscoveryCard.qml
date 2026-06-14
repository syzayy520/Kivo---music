import QtQuick
import "../artwork"
import "../../tokens"

Item {
    id: root
    property string title: ""
    property string subtitle: ""
    property string eyebrow: ""
    property int artVariant: 0
    property url coverUrl: ""

    height: width + 66

    Theme { id: theme }

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
            color: theme.muted
            font.pixelSize: 11
            font.weight: Font.DemiBold
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.title
            color: theme.text
            font.pixelSize: 14
            font.weight: Font.DemiBold
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.subtitle
            color: theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }
}
