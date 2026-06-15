// =============================================================================
// Kivo Music - Sidebar.qml
// Apple Music-level sidebar with SF-style icons and smooth interactions
// =============================================================================

import QtQuick
import QtQuick.Effects
import "../../tokens"

Rectangle {
    id: root
    property string currentPage: "home"
    signal pageRequested(string pageKey, string title)

    color: "#fafafc"

    // Right edge subtle shadow for depth separation
    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#0c000000"
        shadowBlur: 0.35
        shadowHorizontalOffset: 4
        shadowVerticalOffset: 0
    }

    Theme { id: theme }

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
        anchors.bottom: userRow.top
        anchors.leftMargin: 16
        anchors.rightMargin: 12
        anchors.topMargin: 24
        anchors.bottomMargin: 16
        spacing: 20

        // ── Browse Section ──────────────────────────────────
        Column {
            width: parent.width
            spacing: 2

            Text {
                text: "Browse"
                color: theme.muted
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.letterSpacing: 0.5
                leftPadding: 10
                bottomPadding: 6
            }

            NavigationItem { width: parent.width; text: "Home"; pageKey: "home"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Search"; pageKey: "search"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "New"; pageKey: "new"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Radio"; pageKey: "radio"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }

        // ── Library Section ─────────────────────────────────
        Column {
            width: parent.width
            spacing: 2

            Text {
                text: "Library"
                color: theme.muted
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.letterSpacing: 0.5
                leftPadding: 10
                bottomPadding: 6
            }

            NavigationItem { width: parent.width; text: "Recently Added"; pageKey: "recent"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Artists"; pageKey: "artists"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Albums"; pageKey: "albums"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Songs"; pageKey: "songs"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Music Videos"; pageKey: "videos"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }

        // ── Playlists Section ───────────────────────────────
        Column {
            width: parent.width
            spacing: 2

            Text {
                text: "Playlists"
                color: theme.muted
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.letterSpacing: 0.5
                leftPadding: 10
                bottomPadding: 6
            }

            NavigationItem { width: parent.width; text: "Favorites"; pageKey: "favorites"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Focus"; pageKey: "focus"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Made for You"; pageKey: "made"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }

        // ── Settings (at bottom) ───────────────────────────
        Column {
            width: parent.width
            spacing: 2

            Rectangle {
                width: parent.width - 4; height: 1
                color: "#e5e5ea"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Item { width: 1; height: 6 }

            NavigationItem {
                width: parent.width
                text: "Settings"
                pageKey: "settings"
                current: root.currentPage === pageKey
                onClicked: root.pageRequested(pageKey, text)
            }
        }
    }

    // ── User Row ─────────────────────────────────────────────
    Row {
        id: userRow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 18
        anchors.rightMargin: 14
        anchors.bottomMargin: 20
        height: 32
        spacing: 10

        // Avatar circle
        Rectangle {
            width: 28
            height: 28
            radius: 14
            color: theme.accent
            anchors.verticalCenter: parent.verticalCenter

            Text {
                anchors.centerIn: parent
                text: "K"
                color: "#ffffff"
                font.pixelSize: 13
                font.weight: Font.DemiBold
            }
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 0

            Text {
                text: "Kivo Music"
                color: theme.text
                font.pixelSize: 13
                font.weight: Font.DemiBold
            }

            Text {
                text: "Local Library"
                color: theme.muted
                font.pixelSize: 11
            }
        }
    }
}
