// =============================================================================
// Kivo Music - SearchPage.qml
// 职责: 搜索页 —— 真实输入框 + 对真实资料库的实时检索(标题/艺人/专辑)。
// 结果来自 libraryModels.searchTracks(query)(每次按键重算,≤60 条)。
// =============================================================================

import QtQuick
import QtQuick.Controls
import "../../components/search"
import "../../components/common"
import KivoMusic

Item {
    id: root
    readonly property string query: searchField.text
    // Guard libraryModels: the Loader briefly creates this page before the context
    // property is attached, so an unguarded call warns "searchTracks of null".
    readonly property var results:
        (query.trim().length > 0 && typeof libraryModels !== "undefined" && libraryModels)
            ? libraryModels.searchTracks(query) : []

    function _resultPaths() {
        var p = []
        for (var i = 0; i < results.length; i++) p.push(results[i].filePath)
        return p
    }

    // ── Search field + status line (fixed header) ────────────────────────────
    Column {
        id: head
        x: 36
        y: 28
        width: root.width - 72
        spacing: 16

        Rectangle {
            width: Math.min(620, parent.width)
            height: 44
            radius: 22
            color: Theme.panel
            border.color: searchField.activeFocus ? Theme.accent : Theme.line
            border.width: searchField.activeFocus ? 2 : 1
            Behavior on border.color { ColorAnimation { duration: Theme.animFast } }

            Row {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 14
                spacing: 10

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "🔍"; font.pixelSize: 15
                }
                TextField {
                    id: searchField
                    objectName: "searchField"
                    width: parent.width - 32
                    anchors.verticalCenter: parent.verticalCenter
                    placeholderText: qsTr("Search artists, albums, songs")
                    color: Theme.text
                    placeholderTextColor: Theme.faint
                    font.pixelSize: 14
                    font.family: Theme.fontFamily
                    background: Rectangle { color: "transparent" }
                    focus: true
                }
            }
        }

        Text {
            text: root.query.trim().length === 0
                ? qsTr("Search your library by title, artist, or album")
                : root.results.length === 0
                    ? qsTr("No matches for “%1”").arg(root.query.trim())
                    : root.results.length + qsTr(" results")
            color: Theme.muted
            font.pixelSize: 13
            font.family: Theme.fontFamily
        }
    }

    // ── Live results ─────────────────────────────────────────────────────────
    ScrollView {
        anchors.top: head.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        contentWidth: availableWidth
        ScrollBar.vertical: CustomScrollBar { policy: ScrollBar.AsNeeded }

        Column {
            x: 36
            width: root.width - 72
            spacing: 2

            Repeater {
                model: root.results
                SearchResultRow {
                    width: parent.width
                    title: modelData.title
                    detail: modelData.detail
                    meta: modelData.meta
                    artVariant: modelData.artVariant
                    coverUrl: modelData.coverUrl
                    filePath: modelData.filePath
                    // Play this result AND queue the whole result set behind it.
                    onPlayRequested: function(path) {
                        audioController.playTracks(root._resultPaths(), index)
                    }
                }
            }

            Item { width: 1; height: Theme.transportHeight + 72 }
        }
    }
}
