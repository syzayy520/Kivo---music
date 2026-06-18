// =============================================================================
// Kivo Music - AlbumTile.qml
// Apple Music-level album tile with parallax hover and polished play button
// =============================================================================

import QtQuick
import QtQuick.Effects
import "../artwork"
import KivoMusic

Item {
    id: root
    property string title: ""
    property string subtitle: ""
    property string note: ""
    property int artVariant: 0
    property url coverUrl: ""
    property string filePath: ""
    signal playRequested(string filePath)
    signal activated(string albumTitle)  // drill-down to album detail

    height: width + 62


    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        propagateComposedEvents: true
        Accessible.role: Accessible.Button
        Accessible.name: root.title + (root.subtitle.length > 0 ? ", " + root.subtitle : "")
        Accessible.description: qsTr("Play")
        Accessible.onPressAction: { if (root.filePath.length > 0) root.playRequested(root.filePath) }
        onClicked: root.activated(root.title)
    }

    Column {
        anchors.fill: parent
        spacing: 8

        // ── Artwork with parallax scale ────────────────────
        Item {
            width: parent.width
            height: parent.width

            // Subtle lift on hover
            scale: hoverArea.containsMouse ? 1.04 : 1.0
            y: hoverArea.containsMouse ? -2 : 0

            Behavior on scale {
                NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
            }
            Behavior on y {
                NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
            }

            AlbumArtwork {
                id: artwork
                anchors.fill: parent
                radiusValue: 10
                variant: root.artVariant
                sourceUrl: root.coverUrl

                // Enhanced shadow on hover
                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: hoverArea.containsMouse ? Theme.shadowModal : Theme.shadowPanel
                    shadowBlur: hoverArea.containsMouse ? 0.75 : 0.4
                    shadowVerticalOffset: hoverArea.containsMouse ? 14 : 6
                }
            }

            // ── Play button (Apple Music-style circle) ─────
            Rectangle {
                id: playBtn
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 8
                width: 36
                height: 36
                radius: 18
                color: Theme.accent
                opacity: hoverArea.containsMouse ? 0.97 : 0.0
                scale: hoverArea.containsMouse ? 1.0 : 0.8

                Behavior on opacity {
                    NumberAnimation { duration: 180; easing.type: Easing.OutQuad }
                }
                Behavior on scale {
                    NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
                }

                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: Theme.shadowModal
                    shadowBlur: 0.35
                    shadowVerticalOffset: 3
                }

                Canvas {
                    anchors.centerIn: parent
                    width: 16
                    height: 16

                    onPaint: {
                        const ctx = getContext("2d");
                        ctx.clearRect(0, 0, width, height);
                        ctx.fillStyle = Theme.accentInk;
                        ctx.beginPath();
                        ctx.moveTo(width * 0.32, height * 0.22);
                        ctx.lineTo(width * 0.32, height * 0.78);
                        ctx.lineTo(width * 0.78, height * 0.50);
                        ctx.closePath();
                        ctx.fill();
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        mouse.accepted = true
                        if (root.filePath.length > 0) root.playRequested(root.filePath)
                    }
                }
            }
        }

        // ── Title ──────────────────────────────────────────
        Text {
            width: parent.width
            text: root.title
            color: Theme.text
            font.pixelSize: 13
            font.weight: Font.DemiBold
            lineHeight: 1.2
            maximumLineCount: 1
            elide: Text.ElideRight
        }

        // ── Subtitle ───────────────────────────────────────
        Text {
            width: parent.width
            text: root.subtitle
            color: Theme.muted
            font.pixelSize: 12
            lineHeight: 1.2
            maximumLineCount: 1
            elide: Text.ElideRight
        }

        // ── Note badge (if any) ────────────────────────────
        Text {
            visible: root.note.length > 0
            width: parent.width
            text: root.note
            color: Theme.accentText
            font.pixelSize: 11
            font.weight: Font.Medium
            maximumLineCount: 1
            elide: Text.ElideRight
        }
    }
}
