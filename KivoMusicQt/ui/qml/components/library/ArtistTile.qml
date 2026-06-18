import QtQuick
import "../artwork"
import KivoMusic

Item {
    id: root
    property string name: ""
    property string detail: ""
    property int artVariant: 0
    property url coverUrl: ""
    signal activated(string name)

    height: width + 54


    Column {
        anchors.fill: parent
        spacing: 8

        AlbumArtwork {
            id: art
            width: parent.width
            height: parent.width
            radiusValue: width / 2
            variant: root.artVariant
            sourceUrl: root.coverUrl

            // Subtle lift on hover to signal it drills into the artist.
            scale: hover.containsMouse ? 1.03 : 1.0
            Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
        }

        Text {
            width: parent.width
            text: root.name
            color: Theme.text
            font.pixelSize: 14
            font.weight: Font.DemiBold
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: root.detail
            color: Theme.muted
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }
    }

    MouseArea {
        id: hover
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.activated(root.name)
    }
}
