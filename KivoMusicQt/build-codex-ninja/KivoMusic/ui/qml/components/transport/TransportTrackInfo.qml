import QtQuick
import "../artwork"
import "../../tokens"

Item {
    id: root
    property string title: ""
    property string subtitle: ""
    property int artVariant: 0

    height: 48

    Theme { id: theme }

    AlbumArtwork {
        id: art
        width: 44
        height: 44
        radiusValue: 8
        variant: root.artVariant
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
    }

    Column {
        anchors.left: art.right
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 3

        Text {
            width: parent.width
            text: root.title
            color: theme.text
            font.pixelSize: 12
            font.weight: Font.Medium
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.subtitle
            color: theme.muted
            font.pixelSize: 10
            elide: Text.ElideRight
        }
    }
}
