import QtQuick
import "../artwork"
import "../../tokens"

Item {
    id: root
    property string eyebrow: ""
    property string title: ""
    property string subtitle: ""
    property int artVariant: 0
    property url coverUrl: ""
    property bool editorial: false
    readonly property bool hasCover: coverUrl.toString().length > 0

    height: card.y + card.height

    Theme { id: theme }

    Text {
        id: eyebrowText
        width: parent.width
        text: root.eyebrow
        color: theme.muted
        font.pixelSize: 13
        elide: Text.ElideRight
    }

    Rectangle {
        id: card
        width: parent.width
        height: Math.round(parent.width * 1.22)
        y: 22
        radius: 8
        clip: true
        color: "#f3f5f7"

        AlbumArtwork {
            anchors.fill: parent
            radiusValue: 0
            variant: root.artVariant
            sourceUrl: root.coverUrl
        }

        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.00; color: root.editorial ? "#10ffffff" : "#00ffffff" }
                GradientStop { position: 0.52; color: root.hasCover ? "#04111113" : "#08111113" }
                GradientStop { position: 1.00; color: root.hasCover ? "#82111113" : "#94111113" }
            }
        }

        Text {
            text: "SELECTED"
            color: "white"
            opacity: root.editorial ? 0.72 : 0
            font.pixelSize: 10
            font.weight: Font.DemiBold
            anchors.top: parent.top
            anchors.topMargin: 12
            anchors.left: parent.left
            anchors.leftMargin: 14
        }

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 14
            anchors.rightMargin: 14
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 14
            spacing: 5

            Text {
                width: parent.width
                text: root.title
                color: "white"
                font.pixelSize: root.editorial
                    ? (root.hasCover ? 23 : 27)
                    : (root.hasCover ? 17 : 18)
                font.weight: Font.Bold
                lineHeight: 0.98
                wrapMode: Text.WordWrap
                maximumLineCount: 2
                elide: Text.ElideRight
            }

            Text {
                width: parent.width
                text: root.subtitle
                color: "#f3f5f7"
                font.pixelSize: 11
                wrapMode: Text.WordWrap
                maximumLineCount: 2
                elide: Text.ElideRight
            }
        }
    }
}
