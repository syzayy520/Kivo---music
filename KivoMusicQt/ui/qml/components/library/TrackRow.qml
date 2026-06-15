import QtQuick
import "../artwork"
import "../../tokens"

Rectangle {
    id: root
    property string title: ""
    property string artist: ""
    property string album: ""
    property string duration: ""
    property int artVariant: 0
    property url coverUrl: ""
    property string filePath: ""
    signal playRequested(string filePath)

    height: 56
    radius: 7
    color: mouseArea.containsMouse ? "#f5f7f9" : "transparent"

    Behavior on color {
        ColorAnimation { duration: 120; easing.type: Easing.OutQuad }
    }

    Theme { id: theme }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onClicked: {
            if (root.filePath.length > 0)
                root.playRequested(root.filePath)
        }

        onDoubleClicked: {
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
        anchors.right: albumText.left
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2

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
            text: root.artist
            color: theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }

    Text {
        id: albumText
        x: parent.width * 0.50
        width: Math.min(300, parent.width * 0.28)
        text: root.album
        color: theme.muted
        font.pixelSize: 13
        elide: Text.ElideRight
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        width: 48
        text: root.duration
        color: theme.faint
        font.pixelSize: 12
        horizontalAlignment: Text.AlignRight
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        height: 1
        color: theme.line
        opacity: 0.72
        anchors.left: art.right
        anchors.leftMargin: 12
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
    }
}
