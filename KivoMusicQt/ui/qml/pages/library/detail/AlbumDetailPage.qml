// =============================================================================
// Kivo Music - AlbumDetailPage.qml
// 职责: 专辑详情「组合根」—— 返回 + DetailHero（方形封面）+ 曲目列表（序号）。
// 对标 Apple Music 专辑展开图。数据来自 libraryModels.tracksForAlbum()。
// =============================================================================

import QtQuick
import QtQuick.Controls
import "../../../components/library/detail"
import "../../../components/common"
import KivoMusic

Item {
    id: page
    property string albumTitle: ""
    property var tracks: []
    signal backRequested()

    function _paths() {
        var p = []
        for (var i = 0; i < page.tracks.length; i++) p.push(page.tracks[i].filePath)
        return p
    }
    function playAll()     { if (page.tracks.length > 0) audioController.playTracks(_paths(), 0) }
    function playShuffle() {
        var p = _paths()
        for (var i = p.length - 1; i > 0; i--) {
            var j = Math.floor(Math.random() * (i + 1))
            var t = p[i]; p[i] = p[j]; p[j] = t
        }
        if (p.length > 0) audioController.playTracks(p, 0)
    }

    readonly property string albumArtist: tracks.length > 0 ? tracks[0].artist : ""

    // ── Back ─────────────────────────────────────────────────────────────────
    Rectangle {
        id: backBtn
        x: 28; y: 16
        width: backRow.width + 22; height: 34
        radius: 17
        color: backMa.containsMouse ? Theme.panelHover : Theme.panel
        border.color: Theme.line; z: 2
        Behavior on color { ColorAnimation { duration: Theme.animFast } }
        Row {
            id: backRow; anchors.centerIn: parent; spacing: 4
            Text { anchors.verticalCenter: parent.verticalCenter; text: "‹"; color: Theme.text; font.pixelSize: 17 }
            Text { anchors.verticalCenter: parent.verticalCenter; text: qsTr("Albums"); color: Theme.text; font.pixelSize: 13; font.family: Theme.fontFamily }
        }
        MouseArea { id: backMa; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: page.backRequested() }
    }

    ScrollView {
        anchors.fill: parent
        anchors.topMargin: 58
        clip: true
        contentWidth: availableWidth
        ScrollBar.vertical: CustomScrollBar { policy: ScrollBar.AsNeeded }

        Column {
            id: col
            property int margin: 36
            width: page.width
            spacing: 20

            DetailHero {
                x: col.margin
                width: page.width - 2 * col.margin
                eyebrow: page.albumArtist + (page.tracks.length > 0 ? "  ·  " + page.tracks.length + " " + qsTr("songs") : "")
                title: page.albumTitle
                circle: false
                coverUrl: page.tracks.length > 0 ? page.tracks[0].coverUrl : ""
                artVariant: page.tracks.length > 0 ? page.tracks[0].artVariant : 0
                onPlayClicked: page.playAll()
                onShuffleClicked: page.playShuffle()
            }

            Column {
                x: col.margin
                width: page.width - 2 * col.margin
                spacing: 2

                Repeater {
                    model: page.tracks
                    DetailTrackRow {
                        width: parent.width
                        trackNumber: index + 1
                        title: modelData.title
                        duration: modelData.duration || ""
                        current: modelData.title === audioController.title
                              && page.albumTitle === audioController.album
                        onActivated: audioController.playTracks(page._paths(), index)
                    }
                }
            }

            Item { width: 1; height: Theme.transportHeight + 72 }
        }
    }
}
