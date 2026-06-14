import QtQuick
import "../artwork"
import "../../tokens"

Rectangle {
    id: root
    property string title: ""
    property string artist: ""
    property string context: ""
    property string timeText: ""
    property real progress: 0
    property int artVariant: 0
    property url coverUrl: ""
    readonly property string detailText: timeText.length > 0
        ? context + " - " + timeText
        : context

    height: 92
    radius: 9
    color: "#ffffff"
    border.color: "#e7e3e1"

    Theme { id: theme }

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
            color: theme.text
            font.pixelSize: 15
            font.weight: Font.DemiBold
            elide: Text.ElideRight
        }

        Text {
            text: root.artist
            color: theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }

        Text {
            text: root.detailText
            color: theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }

        Rectangle {
            width: parent.width
            height: 2
            radius: 1
            color: "#dfe3e8"
            visible: root.progress > 0

            Rectangle {
                width: parent.width * Math.max(0, Math.min(1, root.progress))
                height: parent.height
                radius: 1
                color: theme.accent
            }
        }
    }

    Rectangle {
        id: play
        width: 34
        height: 34
        radius: 17
        color: "#f7f8fa"
        border.color: "#e2e5e9"
        anchors.right: parent.right
        anchors.rightMargin: 14
        anchors.verticalCenter: parent.verticalCenter

        Canvas {
            anchors.fill: parent
            onPaint: {
                const ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height);
                ctx.fillStyle = theme.ink;
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
        color: theme.muted
        font.pixelSize: 16
        anchors.right: parent.right
        anchors.rightMargin: 17
        anchors.top: parent.top
        anchors.topMargin: 5
    }
}
