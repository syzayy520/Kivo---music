// =============================================================================
// Kivo Music - Sidebar.qml
// 职责: 左侧导航栏「容器」—— 接入主题(暗/亮一致)、品牌、分区导航、设置入口。
// 收口为「专注的本地 Hi-Fi 播放器」:只保留核心条目,去掉流媒体式杂项。
// 单一职责:本文件只负责布局与接线,图标/选中样式在 NavigationItem。
// =============================================================================

import QtQuick
import KivoMusic

Rectangle {
    id: root
    property string currentPage: "home"
    signal pageRequested(string pageKey, string title)

    color: Theme.sidebar

    // 用一条主题感知的发丝右边线做层次分隔,取代之前的硬编码投影(暗色下会发灰)。
    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: Theme.sidebarBorder
    }


    // ── Brand ───────────────────────────────────────────────
    SidebarBrand {
        id: brand
        width: parent.width - 32
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 24
    }

    // ── Navigation ───────────────────────────────────────────
    Column {
        id: navColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: brand.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 16
        anchors.rightMargin: 12
        anchors.topMargin: 26
        anchors.bottomMargin: 20
        spacing: 22

        // ── 浏览 ────────────────────────────────────────────
        Column {
            width: parent.width
            spacing: 2

            Text {
                text: qsTr("Browse")
                color: Theme.muted
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.letterSpacing: 0.5
                leftPadding: 10
                bottomPadding: 6
            }

            NavigationItem { width: parent.width; text: qsTr("Home"); kind: "home"; pageKey: "home"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: qsTr("Search"); kind: "search"; pageKey: "search"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }

        // ── 资料库 ──────────────────────────────────────────
        Column {
            width: parent.width
            spacing: 2

            Text {
                text: qsTr("Library")
                color: Theme.muted
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.letterSpacing: 0.5
                leftPadding: 10
                bottomPadding: 6
            }

            NavigationItem { width: parent.width; text: qsTr("Recently Added"); kind: "recent"; pageKey: "recent"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: qsTr("Artists"); kind: "artist"; pageKey: "artists"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: qsTr("Albums"); kind: "album"; pageKey: "albums"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: qsTr("Songs"); kind: "song"; pageKey: "songs"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }

        // ── 播放列表 ────────────────────────────────────────
        Column {
            width: parent.width
            spacing: 2

            Text {
                text: qsTr("Playlists")
                color: Theme.muted
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.letterSpacing: 0.5
                leftPadding: 10
                bottomPadding: 6
            }

            NavigationItem { width: parent.width; text: qsTr("Favorites"); kind: "favorite"; pageKey: "favorites"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }

        // ── 设置(底部) ───────────────────────────────────
        Column {
            width: parent.width
            spacing: 2

            Rectangle {
                width: parent.width - 4; height: 1
                color: Theme.line
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Item { width: 1; height: 6 }

            NavigationItem {
                width: parent.width
                text: qsTr("Settings")
                kind: "settings"
                pageKey: "settings"
                current: root.currentPage === pageKey
                onClicked: root.pageRequested(pageKey, text)
            }
        }
    }
}
