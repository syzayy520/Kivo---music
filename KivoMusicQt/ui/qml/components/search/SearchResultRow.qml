import QtQuick
import "../artwork"
import KivoMusic

Rectangle {
    id: root
    property string title: ""
    property string detail: ""
    property string meta: ""
    property int artVariant: 0
    property url coverUrl: ""
    property string filePath: ""
    signal playRequested(string filePath)

    height: 58
    radius: 7
    color: mouseArea.containsMouse ? Theme.lineSubtle : "transparent"


    Behavior on color {
        ColorAnimation { duration: 120; easing.type: Easing.OutQuad }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            if (root.filePath.length > 0)
                root.playRequested(root.filePath)
        }
    }

    AlbumArtwork {
        id: art
        width: 42
        height: 42
        radiusValue: 5
        variant: root.artVariant
        sourceUrl: root.coverUrl
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
    }

    Column {
        anchors.left: art.right
        anchors.leftMargin: 12
        anchors.right: metaText.left
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2

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
            text: root.detail
            color: Theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }

    Text {
        id: metaText
        width: 160
        text: root.meta
        color: Theme.muted
        font.pixelSize: 12
        horizontalAlignment: Text.AlignRight
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        elide: Text.ElideRight
    }
}
