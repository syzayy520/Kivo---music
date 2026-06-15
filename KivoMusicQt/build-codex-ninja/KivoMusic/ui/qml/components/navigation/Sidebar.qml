import QtQuick
import "../../tokens"

Rectangle {
    id: root
    property string currentPage: "home"
    signal pageRequested(string pageKey, string title)

    color: "#fafbfc"

    Theme { id: theme }

    SidebarBrand {
        id: brand
        width: parent.width - 32
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 20
    }

    Column {
        id: navColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: brand.bottom
        anchors.bottom: userRow.top
        anchors.leftMargin: 18
        anchors.rightMargin: 14
        anchors.topMargin: 20
        anchors.bottomMargin: 18
        spacing: 18

        Column {
            width: parent.width
            spacing: 4

            NavigationItem { width: parent.width; text: "Search"; pageKey: "search"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Home"; pageKey: "home"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "New"; pageKey: "new"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Radio"; pageKey: "radio"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }

        Column {
            width: parent.width
            spacing: 4

            Text {
                text: "Library"
                color: theme.muted
                font.pixelSize: 12
                font.weight: Font.DemiBold
                leftPadding: 2
                bottomPadding: 4
            }

            NavigationItem { width: parent.width; text: "Pins"; pageKey: "pins"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Recently Added"; pageKey: "recent"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Artists"; pageKey: "artists"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Albums"; pageKey: "albums"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Songs"; pageKey: "songs"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Music Videos"; pageKey: "videos"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Made for You"; pageKey: "made"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }

        Column {
            width: parent.width
            spacing: 4

            Text {
                text: "Playlists"
                color: theme.muted
                font.pixelSize: 12
                font.weight: Font.DemiBold
                leftPadding: 2
                bottomPadding: 4
            }

            NavigationItem { width: parent.width; text: "Favorite Songs"; pageKey: "favorites"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
            NavigationItem { width: parent.width; text: "Work Focus"; pageKey: "focus"; current: root.currentPage === pageKey; onClicked: root.pageRequested(pageKey, text) }
        }
    }

    Row {
        id: userRow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 18
        anchors.rightMargin: 14
        anchors.bottomMargin: 18
        height: 28
        spacing: 9

        Rectangle {
            width: 24
            height: 24
            radius: 12
            color: "#eceff3"
            anchors.verticalCenter: parent.verticalCenter

            Text {
                anchors.centerIn: parent
                text: "L"
                color: theme.ink
                font.pixelSize: 12
                font.weight: Font.DemiBold
            }
        }

        Text {
            width: parent.width - 34
            text: "Local Library"
            color: theme.text
            font.pixelSize: 12
            font.weight: Font.DemiBold
            elide: Text.ElideRight
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
