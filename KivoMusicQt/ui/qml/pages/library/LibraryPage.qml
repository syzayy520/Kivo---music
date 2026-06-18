import QtQuick
import QtQuick.Controls
import "../../components/content"
import "../../components/library"
import "detail"
import KivoMusic

Item {
    id: root
    objectName: "libraryPage"
    property string mode: "albums"
    // Drill-down: non-empty → detail view covers the grid. Local to this page.
    property string detailArtist: ""
    property string detailAlbum: ""

    readonly property bool artistMode: mode === "artists"
    readonly property bool songMode: mode === "songs"
    readonly property bool videoMode: mode === "videos"
    readonly property bool favoriteMode: mode === "favorites"
    readonly property bool focusMode: mode === "focus"
    readonly property bool pinMode: mode === "pins"
    readonly property bool recentMode: mode === "recent"
    readonly property bool trackMode: root.songMode || root.videoMode || root.favoriteMode || root.focusMode
    readonly property var gridItems: root.pinMode
        ? libraryModels.pins
        : root.recentMode
            ? libraryModels.recent
            : libraryModels.albums
    readonly property var trackItems: root.videoMode
        ? libraryModels.videos
        : root.favoriteMode
            ? libraryModels.favorites
            : root.focusMode
                ? libraryModels.focus
                : libraryModels.songs
    readonly property string summaryText: {
        if (root.videoMode)
            return root.trackItems.count + qsTr(" videos discovered");
        if (root.trackMode)
            return root.trackItems.count + qsTr(" songs discovered");
        if (root.pinMode)
            return libraryModels.pins.count + qsTr(" pinned library items");
        if (root.recentMode)
            return libraryModels.recent.count + qsTr(" recently discovered items");
        if (root.artistMode)
            return libraryModels.artists.count + qsTr(" artists in this library");
        return libraryModels.albums.count + qsTr(" items across local and NAS sources");
    }
    readonly property string sortText: root.trackMode ? qsTr("Sort by Album") : qsTr("Recently Added")


    ScrollView {
        id: scroll
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth
        visible: root.detailArtist === "" && root.detailAlbum === ""

        Column {
            id: pageColumn
            property int contentMargin: 36
            readonly property real contentWidth: width - contentMargin * 2

            width: scroll.availableWidth
            spacing: 18

            Item {
                x: pageColumn.contentMargin
                width: pageColumn.contentWidth
                height: 32

                Text {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: root.summaryText
                    color: Theme.text
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                }

                Text {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: root.sortText
                    color: Theme.muted
                    font.pixelSize: 13
                }
            }

            Grid {
                x: pageColumn.contentMargin
                width: pageColumn.contentWidth
                columns: 6
                columnSpacing: 24
                rowSpacing: 30
                visible: !root.artistMode && !root.trackMode

                Repeater {
                    model: root.gridItems

                    AlbumTile {
                        width: (parent.width - 5 * 24) / 6
                        title: model.title
                        subtitle: model.subtitle
                        note: model.note
                        artVariant: model.artVariant
                        coverUrl: model.coverUrl
                        filePath: model.filePath
                        onPlayRequested: function(path) {
                            audioController.openFile(path)
                        }
                        onActivated: function(albumTitle) {
                            root.detailAlbum = albumTitle
                        }
                    }
                }
            }

            Grid {
                x: pageColumn.contentMargin
                width: pageColumn.contentWidth
                columns: 6
                columnSpacing: 28
                rowSpacing: 24
                visible: root.artistMode

                Repeater {
                    model: libraryModels.artists

                    ArtistTile {
                        width: (parent.width - 140) / 6
                        name: model.name
                        detail: model.detail
                        artVariant: model.artVariant
                        coverUrl: model.coverUrl
                        onActivated: function(name) { root.detailArtist = name }
                    }
                }
            }

            Column {
                x: pageColumn.contentMargin
                width: pageColumn.contentWidth
                spacing: 2
                visible: root.trackMode

                Repeater {
                    model: root.trackItems

                    TrackRow {
                        required property int index
                        required property var model
                        width: parent.width
                        trackNumber: root.trackMode ? index + 1 : 0
                        title: model.title
                        artist: model.artist
                        album: model.album
                        duration: model.duration
                        artVariant: model.artVariant
                        coverUrl: model.coverUrl
                        filePath: model.filePath
                        playing: {
                            if (!audioController.playQueueModel) return false;
                            const qi = audioController.playQueueModel.currentIndex;
                            return qi >= 0 && audioController.playQueueModel.getTrackAt(qi) === model.filePath;
                        }
                        onPlayRequested: {
                            const kind = root.videoMode ? "videos"
                                       : root.favoriteMode ? "favorites"
                                       : root.focusMode ? "focus" : "songs"
                            audioController.playTracks(
                                libraryModels.trackFilePaths(kind), index)
                        }
                    }
                }
            }

            Item {
                width: 1
                height: 190
            }
        }
    }

    // ── Artist drill-down (covers the grid; Back clears detailArtist) ─────────
    ArtistDetailPage {
        anchors.fill: parent
        visible: root.detailArtist !== ""
        artistName: root.detailArtist
        tracks: root.detailArtist !== ""
            ? libraryModels.tracksForArtist(root.detailArtist)
            : []
        onBackRequested: root.detailArtist = ""
    }

    // ── Album drill-down (covers the grid; Back clears detailAlbum) ─────────
    AlbumDetailPage {
        anchors.fill: parent
        visible: root.detailAlbum !== ""
        albumTitle: root.detailAlbum
        tracks: root.detailAlbum !== ""
            ? libraryModels.tracksForAlbum(root.detailAlbum)
            : []
        onBackRequested: root.detailAlbum = ""
    }
}
