import QtQuick
import "../artwork"
import KivoMusic

Rectangle {
    id: root
    property string title: ""
    property string artist: ""
    property string context: ""
    property string timeText: ""
    property real progress: 0
    property int artVariant: 0
    property url coverUrl: ""
    property string filePath: ""
    readonly property string detailText: timeText.length > 0
        ? context + " - " + timeText
        : context
    signal playRequested(string filePath)

    height: 92
    radius: 9
    color: cardMouse.containsMouse ? Theme.cardHover : Theme.card
    border.color: Theme.cardBorder
    Behavior on color { ColorAnimation { duration: 120 } }


    MouseArea {
        id: cardMouse
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
        width: 92
        height: 92
        radiusValue: 9
        variant: root.artVariant
        sourceUrl: root.coverUrl
        anchors.left: parent.left
        anchors.top: parent.top
    }

    Column {
        anchors.left: art.right
        anchors.leftMargin: 16
        anchors.right: play.left
        anchors.rightMargin: 18
        anchors.verticalCenter: parent.verticalCenter
        spacing: 4

        Text {
            text: root.title
            color: Theme.text
            font.pixelSize: 15
            font.weight: Font.DemiBold
            elide: Text.ElideRight
        }

        Text {
            text: root.artist
            color: Theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }

        Text {
            text: root.detailText
            color: Theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }

        Rectangle {
            width: parent.width
            height: 2
            radius: 1
            color: Theme.transportTrack
            visible: root.progress > 0

            Rectangle {
                width: parent.width * Math.max(0, Math.min(1, root.progress))
                height: parent.height
                radius: 1
                color: Theme.accent
            }
        }
    }

    Rectangle {
        id: play
        width: 34
        height: 34
        radius: 17
        color: playMouse.containsMouse ? Theme.accentSoft : Theme.panelSoft
        border.color: Theme.line
        anchors.right: parent.right
        anchors.rightMargin: 14
        anchors.verticalCenter: parent.verticalCenter
        Behavior on color { ColorAnimation { duration: 120 } }

        MouseArea {
            id: playMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (root.filePath.length > 0)
                    root.playRequested(root.filePath)
            }
        }

        Canvas {
            anchors.fill: parent
            onPaint: {
                const ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height);
                ctx.fillStyle = Theme.accentText;
                ctx.beginPath();
                ctx.moveTo(width * 0.42, height * 0.32);
                ctx.lineTo(width * 0.42, height * 0.68);
                ctx.lineTo(width * 0.68, height * 0.50);
                ctx.closePath();
                ctx.fill();
            }
        }
    }

    Text {
        text: "..."
        color: Theme.muted
        font.pixelSize: 16
        anchors.right: parent.right
        anchors.rightMargin: 17
        anchors.top: parent.top
        anchors.topMargin: 5
    }
}
