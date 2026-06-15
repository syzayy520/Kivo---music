// =============================================================================
// Kivo Music - Sidebar.qml
// Quiet commercial macOS sidebar, local-library first
// =============================================================================

import QtQuick
import QtQuick.Effects
import "../../tokens"

Rectangle {
    id: root
    property string currentPage: "home"
    signal pageRequested(string pageKey, string title)

    Theme { id: theme }

    color: theme.sidebar

    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: "#dfe0e6"
        opacity: 0.72
    }

    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#09000000"
        shadowBlur: 0.22
        shadowHorizontalOffset: 3
        shadowVerticalOffset: 0
    }

    SidebarBrand {
        id: brand
        width: parent.width - 38
        anchors.left: parent.left
        anchors.leftMargin: 22
        anchors.top: parent.top
        anchors.topMargin: 22
    }

    Column {
        id: navColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: brand.bottom
        anchors.bottom: footer.top
        anchors.leftMargin: 16
        anchors.rightMargin: 14
        anchors.topMargin: 24
        anchors.bottomMargin: 18
        spacing: 22

        Column {
            width: parent.width
            spacing: 4

            Text {
                text: "BROWSE"
                color: theme.faint
                font.pixelSize: 10
                font.weight: Font.DemiBold
                font.letterSpacing: 1.1
                leftPadding: 10
                bottomPadding: 6
            }

            NavigationItem { width: parent.width; text: "Home"; pageKey: "home"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Search"; pageKey: "search"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "New Music"; pageKey: "new"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }

        Column {
            width: parent.width
            spacing: 4

            Text {
                text: "LIBRARY"
                color: theme.faint
                font.pixelSize: 10
                font.weight: Font.DemiBold
                font.letterSpacing: 1.1
                leftPadding: 10
                bottomPadding: 6
            }

            NavigationItem { width: parent.width; text: "Recently Added"; pageKey: "recent"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Artists"; pageKey: "artists"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Albums"; pageKey: "albums"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Songs"; pageKey: "songs"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }

        Column {
            width: parent.width
            spacing: 4

            Text {
                text: "PLAYLISTS"
                color: theme.faint
                font.pixelSize: 10
                font.weight: Font.DemiBold
                font.letterSpacing: 1.1
                leftPadding: 10
                bottomPadding: 6
            }

            NavigationItem { width: parent.width; text: "Favorites"; pageKey: "favorites"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Focus"; pageKey: "focus"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Made for You"; pageKey: "made"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }
    }

    Column {
        id: footer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 16
        anchors.rightMargin: 14
        anchors.bottomMargin: 18
        spacing: 12

        Rectangle {
            width: parent.width
            height: 1
            color: theme.line
            opacity: 0.7
        }

        NavigationItem {
            width: parent.width
            text: "Settings"
            pageKey: "settings"
            current: root.currentPage === pageKey
            onClicked: root.pageRequested(pageKey, text)
        }

        Row {
            width: parent.width
            height: 38
            spacing: 10

            Rectangle {
                width: 30
                height: 30
                radius: 9
                anchors.verticalCenter: parent.verticalCenter
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#ff6a7a" }
                    GradientStop { position: 1.0; color: "#fa233b" }
                }

                Text {
                    anchors.centerIn: parent
                    text: "K"
                    color: "#ffffff"
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 1

                Text {
                    text: "Kivo Music"
                    color: theme.text
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                }

                Text {
                    text: "Local library"
                    color: theme.muted
                    font.pixelSize: 11
                }
            }
        }
    }
}
