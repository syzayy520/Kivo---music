import QtQuick
import QtQuick.Controls
import "../../components/content"
import "../../components/library"
import "../../tokens"

Item {
    id: root
    property string mode: "albums"

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
            return root.trackItems.count + " videos discovered";
        if (root.trackMode)
            return root.trackItems.count + " songs discovered";
        if (root.pinMode)
            return libraryModels.pins.count + " pinned library items";
        if (root.recentMode)
            return libraryModels.recent.count + " recently discovered items";
        if (root.artistMode)
            return libraryModels.artists.count + " artists in this library";
        return libraryModels.albums.count + " items across local and NAS sources";
    }
    readonly property string sortText: root.trackMode ? "Sort by Album" : "Recently Added"

    Theme { id: theme }

    ScrollView {
        id: scroll
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth

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
                    color: theme.text
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                }

                Text {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: root.sortText
                    color: theme.muted
                    font.pixelSize: 13
                }
            }

            Grid {
                x: pageColumn.contentMargin
                width: pageColumn.contentWidth
                columns: 8
                columnSpacing: 22
                rowSpacing: 24
                visible: !root.artistMode && !root.trackMode

                Repeater {
                    model: root.gridItems

                    AlbumTile {
                        width: (parent.width - 154) / 8
                        title: model.title
                        subtitle: model.subtitle
                        note: model.note
                        artVariant: model.artVariant
                        coverUrl: model.coverUrl
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
                        width: parent.width
                        title: model.title
                        artist: model.artist
                        album: model.album
                        duration: model.duration
                        artVariant: model.artVariant
                        coverUrl: model.coverUrl
                    }
                }
            }

            Item {
                width: 1
                height: 190
            }
        }
    }
}
