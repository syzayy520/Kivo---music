import QtQuick
import "../artwork"
import "../../tokens"

Item {
    id: root
    property string name: ""
    property string detail: ""
    property int artVariant: 0
    property url coverUrl: ""

    height: width + 54

    Theme { id: theme }

    Column {
        anchors.fill: parent
        spacing: 8

        AlbumArtwork {
            width: parent.width
            height: parent.width
            radiusValue: width / 2
            variant: root.artVariant
            sourceUrl: root.coverUrl
        }

        Text {
            width: parent.width
            text: root.name
            color: theme.text
            font.pixelSize: 14
            font.weight: Font.DemiBold
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.detail
            color: theme.muted
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }
    }
}
