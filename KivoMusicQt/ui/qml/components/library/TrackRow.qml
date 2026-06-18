import QtQuick
import "../artwork"
import KivoMusic

Rectangle {
    id: root
    property string title: ""
    property string artist: ""
    property string album: ""
    property string duration: ""
    property int artVariant: 0
    property url coverUrl: ""
    property string filePath: ""
    property bool playing: false
    property int trackNumber: 0  // 0 = show artwork; >0 = show number (compact mode)
    signal playRequested(string filePath)

    height: 56
    radius: 7
    color: mouseArea.containsMouse ? Theme.panelHover : (root.playing ? Theme.panelSoft : "transparent")

    Behavior on color { ColorAnimation { duration: 120; easing.type: Easing.OutQuad } }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked:       { if (root.filePath.length > 0) root.playRequested(root.filePath) }
        onDoubleClicked: { if (root.filePath.length > 0) root.playRequested(root.filePath) }
    }

    // Artwork + now-playing overlay (or track number in compact mode)
    Item {
        id: artSlot
        width: 42; height: 42
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter

        // Compact mode: show track number instead of artwork
        Text {
            anchors.centerIn: parent
            visible: root.trackNumber > 0
            text: root.trackNumber
            color: root.playing ? Theme.accent : Theme.muted
            font.pixelSize: 14
            font.weight: root.playing ? Font.DemiBold : Font.Normal
        }

        AlbumArtwork {
            anchors.fill: parent
            radiusValue: 5
            variant: root.artVariant
            sourceUrl: root.coverUrl
            visible: root.trackNumber === 0
        }

        // ♪ pulse when this track is the current track (only in artwork mode)
        Rectangle {
            anchors.fill: parent
            radius: 5
            color: "#CC000000"
            visible: root.playing && root.trackNumber === 0

            Text {
                anchors.centerIn: parent
                text: "♪"
                font.pixelSize: 18
                color: Theme.accent
                SequentialAnimation on opacity {
                    running: root.playing && root.trackNumber === 0
                    loops: Animation.Infinite
                    NumberAnimation { to: 0.4; duration: 900; easing.type: Easing.InOutSine }
                    NumberAnimation { to: 1.0; duration: 900; easing.type: Easing.InOutSine }
                }
                onVisibleChanged: if (!visible) opacity = 1.0
            }
        }
    }

    Column {
        anchors.left: artSlot.right
        anchors.leftMargin: 12
        anchors.right: albumText.left
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2

        Text {
            width: parent.width
            text: root.title
            color: root.playing ? Theme.accent : Theme.text
            font.pixelSize: 14
            font.weight: root.playing ? Font.DemiBold : Font.Normal
            elide: Text.ElideRight
        }
        Text {
            width: parent.width
            text: root.artist
            color: root.playing ? Theme.accentText : Theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }

    Text {
        id: albumText
        x: parent.width * 0.50
        width: Math.min(300, parent.width * 0.28)
        text: root.album
        color: Theme.muted
        font.pixelSize: 13
        elide: Text.ElideRight
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        width: 48
        text: root.duration
        color: root.playing ? Theme.accent : Theme.faint
        font.pixelSize: 12
        horizontalAlignment: Text.AlignRight
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        height: 1
        color: Theme.line
        opacity: 0.72
        anchors.left: artSlot.right
        anchors.leftMargin: 12
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
    }
}
