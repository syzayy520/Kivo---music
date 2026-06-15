// =============================================================================
// Kivo Music - TopPickCard.qml
// Apple Music-level editorial card with gradient overlay and refined typography
// =============================================================================

import QtQuick
import QtQuick.Effects
import "../artwork"
import "../../tokens"

Item {
    id: root
    property string eyebrow: ""
    property string title: ""
    property string subtitle: ""
    property int artVariant: 0
    property url coverUrl: ""
    property string filePath: ""
    property bool editorial: false
    readonly property bool hasCover: coverUrl.toString().length > 0
    signal playRequested(string filePath)

    height: card.y + card.height + 8

    Theme { id: theme }

    // ── Eyebrow ────────────────────────────────────────────────
    Text {
        id: eyebrowText
        width: parent.width
        text: root.eyebrow
        color: theme.muted
        font.pixelSize: 12
        font.weight: Font.Medium
        elide: Text.ElideRight
        opacity: 0.9
    }

    // ── Card ───────────────────────────────────────────────────
    Rectangle {
        id: card
        width: parent.width
        height: Math.round(parent.width * 1.28)
        y: 20
        radius: 10
        clip: true
        color: "#f0f1f4"

        // Subtle lift on hover
        scale: hoverArea.containsMouse ? 1.02 : 1.0

        Behavior on scale {
            NumberAnimation { duration: 250; easing.type: Easing.OutCubic }
        }

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: hoverArea.containsMouse ? "#2a000000" : "#14000000"
            shadowBlur: hoverArea.containsMouse ? 0.55 : 0.3
            shadowVerticalOffset: hoverArea.containsMouse ? 10 : 4
        }

        AlbumArtwork {
            anchors.fill: parent
            radiusValue: 0
            variant: root.artVariant
            sourceUrl: root.coverUrl
        }

        // ── Gradient overlay ───────────────────────────────
        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#00000000" }
                GradientStop { position: 0.45; color: root.hasCover ? "#00061113" : "#000a1113" }
                GradientStop { position: 0.75; color: root.hasCover ? "#45111318" : "#4a111318" }
                GradientStop { position: 1.0; color: root.hasCover ? "#8a111318" : "#95111318" }
            }
        }

        // ── Editorial badge ────────────────────────────────
        Text {
            text: "✦ Editor's Pick"
            color: "#ffffff"
            opacity: root.editorial ? 0.9 : 0
            font.pixelSize: 10
            font.weight: Font.DemiBold
            font.letterSpacing: 0.8
            anchors.top: parent.top
            anchors.topMargin: 14
            anchors.left: parent.left
            anchors.leftMargin: 16

            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
        }

        // ── Text overlay ───────────────────────────────────
        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
            spacing: 6

            Text {
                width: parent.width
                text: root.title
                color: "#ffffff"
                font.pixelSize: root.editorial ? (root.hasCover ? 24 : 28) : (root.hasCover ? 17 : 18)
                font.weight: Font.Bold
                lineHeight: 0.96
                wrapMode: Text.WordWrap
                maximumLineCount: 2
                elide: Text.ElideRight
            }

            Text {
                width: parent.width
                text: root.subtitle
                color: "#e8eaed"
                font.pixelSize: 11
                font.weight: Font.Medium
                wrapMode: Text.WordWrap
                maximumLineCount: 1
                elide: Text.ElideRight
                opacity: 0.85
            }
        }

        // ── Hover interaction ──────────────────────────────
        MouseArea {
            id: hoverArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (root.filePath.length > 0)
                    root.playRequested(root.filePath)
            }
        }
    }
}
