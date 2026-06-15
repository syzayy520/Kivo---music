import QtQuick
import "../tokens"
import "../components/navigation"
import "../components/shell"
import "../components/transport"
import "../pages/discovery"
import "../pages/home"
import "../pages/library"
import "../pages/search"

Item {
    id: root
    property string currentPage: "home"
    signal minimizeRequested()
    signal maximizeRequested()
    signal closeRequested()
    signal dragRequested()
    readonly property string currentTitle: {
        if (currentPage === "home")
            return "Home";
        if (currentPage === "artists")
            return "Artists";
        if (currentPage === "albums")
            return "Albums";
        if (currentPage === "songs")
            return "Songs";
        if (currentPage === "new")
            return "New";
        if (currentPage === "radio")
            return "Radio";
        if (currentPage === "made")
            return "Made for You";
        if (currentPage === "search")
            return "Search";
        if (currentPage === "pins")
            return "Pins";
        if (currentPage === "favorites")
            return "Favorite Songs";
        if (currentPage === "focus")
            return "Work Focus";
        if (currentPage === "videos")
            return "Music Videos";
        if (currentPage === "recent")
            return "Recently Added";
        return "Library";
    }

    Theme { id: theme }

    Rectangle {
        anchors.fill: parent
        color: theme.page
    }

    Sidebar {
        id: sidebar
        width: theme.railWidth
        currentPage: root.currentPage
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        onPageRequested: root.currentPage = pageKey
    }

    Item {
        id: content
        anchors.top: parent.top
        anchors.left: sidebar.right
        anchors.right: parent.right
        anchors.bottom: transport.top

        TopBar {
            id: topBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 78
            pageTitle: root.currentTitle
            onMinimizeRequested: root.minimizeRequested()
            onMaximizeRequested: root.maximizeRequested()
            onCloseRequested: root.closeRequested()
            onDragRequested: root.dragRequested()
        }

        Loader {
            id: pageLoader
            anchors.top: topBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            // 页面切换淡入淡出动画
            opacity: 0

            PropertyAnimation {
                id: fadeIn
                target: pageLoader
                property: "opacity"
                from: 0
                to: 1
                duration: 250
                easing.type: Easing.OutQuad
            }

            PropertyAnimation {
                id: fadeOut
                target: pageLoader
                property: "opacity"
                from: 1
                to: 0
                duration: 200
                easing.type: Easing.InQuad

                onFinished: {
                    pageLoader.sourceComponent = getPageComponent();
                    fadeIn.start();
                }
            }

            function getPageComponent() {
                if (root.currentPage === "home")
                    return homePageComponent;
                if (root.currentPage === "search")
                    return searchPageComponent;
                if (root.currentPage === "new" || root.currentPage === "radio" || root.currentPage === "made")
                    return discoveryPageComponent;
                return libraryPageComponent;
            }

            sourceComponent: homePageComponent

            Component.onCompleted: {
                fadeIn.start();
            }

            Connections {
                target: root
                function onCurrentPageChanged() {
                    if (pageLoader.opacity === 1) {
                        fadeOut.start();
                    } else {
                        pageLoader.sourceComponent = pageLoader.getPageComponent();
                        fadeIn.start();
                    }
                }
            }
        }
    }

    Component {
        id: homePageComponent
        HomePage {}
    }

    Component {
        id: libraryPageComponent
        LibraryPage {
            mode: root.currentPage
        }
    }

    Component {
        id: searchPageComponent
        SearchPage {}
    }

    Component {
        id: discoveryPageComponent
        DiscoveryPage {
            mode: root.currentPage
        }
    }

    TransportBar {
        id: transport
        anchors.left: sidebar.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: theme.transportHeight + 54
    }
}
