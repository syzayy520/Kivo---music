import QtQuick
import KivoMusic
import "../components/navigation"
import "../components/shell"
import "../components/transport"
import "../components/transport/queue"
import "../pages/discovery"
import "../pages/home"
import "../pages/library"
import "../pages/search"
import "../pages/settings"

Item {
    id: root
    property string currentPage: "home"
    signal minimizeRequested()
    signal maximizeRequested()
    signal closeRequested()
    signal dragRequested()
    readonly property string currentTitle: {
        if (currentPage === "home") return qsTr("Home");
        if (currentPage === "recent") return qsTr("Recently Added");
        if (currentPage === "artists") return qsTr("Artists");
        if (currentPage === "albums") return qsTr("Albums");
        if (currentPage === "songs") return qsTr("Songs");
        if (currentPage === "favorites") return qsTr("Favorites");
        if (currentPage === "search") return qsTr("Search");
        if (currentPage === "settings") return qsTr("Settings");
        return qsTr("Library");
    }

    // Helper: focus the search field after the search page loads.
    function _focusSearchField() {
        Qt.callLater(function() {
            if (!pageLoader.item) return
            // SearchPage's TextField has objectName "searchField" and focus:true,
            // so it auto-focuses on load. This is a fallback to force it.
            const field = pageLoader.item.children.length > 0
                ? _findObjectByName(pageLoader.item, "searchField") : null
            if (field) field.forceActiveFocus()
        })
    }

    function _findObjectByName(parent, name) {
        if (!parent) return null
        if (parent.objectName === name) return parent
        for (let i = 0; i < parent.children.length; i++) {
            const found = _findObjectByName(parent.children[i], name)
            if (found) return found
        }
        return null
    }


    Rectangle {
        anchors.fill: parent
        color: Theme.page
    }

    // File drop area for drag-and-drop support
    FileDropArea {
        id: fileDropArea
    }

    Sidebar {
        id: sidebar
        width: Theme.railWidth
        currentPage: root.currentPage
        anchors.top: parent.top
        anchors.bottom: transport.top
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
            onSearchRequested: {
                root.currentPage = "search"
                _focusSearchField()
            }
        }

        // ── Page Loader with Apple Music transitions ──────────
        // D9 修复: 原实现用 Qt.createQmlObject 拼接字符串创建动画,target 对象引用可能非法。
        // 改为声明式 SequentialAnimation + Translate 位移(不与 anchors 冲突)。
        Loader {
            id: pageLoader
            anchors.top: topBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            asynchronous: true
            opacity: 0

            // Translate Y offset 用于滑入/滑出; 与 anchors 正交, 无冲突警告。
            transform: Translate { id: pageSlide; y: 14 }

            property string _pendingPage: ""

            // ── 淡出动画 ─────────────────────────────────────
            SequentialAnimation {
                id: _fadeOut
                PauseAnimation { duration: 30 }
                ParallelAnimation {
                    NumberAnimation {
                        target: pageLoader; property: "opacity"
                        to: 0; duration: 180; easing.type: Easing.InQuad
                    }
                    NumberAnimation {
                        target: pageSlide; property: "y"
                        to: 14; duration: 180; easing.type: Easing.InQuad
                    }
                }
                ScriptAction { script: pageLoader._onFadeOutDone() }
            }

            // ── 淡入动画 ─────────────────────────────────────
            SequentialAnimation {
                id: _fadeIn
                PauseAnimation { duration: 40 }
                ParallelAnimation {
                    NumberAnimation {
                        target: pageLoader; property: "opacity"
                        from: 0; to: 1; duration: 280; easing.type: Easing.OutCubic
                    }
                    NumberAnimation {
                        target: pageSlide; property: "y"
                        from: 14; to: 0; duration: 280; easing.type: Easing.OutCubic
                    }
                }
                ScriptAction { script: pageLoader._pendingPage = "" }
            }

            function switchPage(pageKey) {
                if (_pendingPage !== "") return;
                _pendingPage = pageKey;
                _fadeOut.start();
            }

            function _onFadeOutDone() {
                const newSource = getPageComponent();
                if (newSource !== sourceComponent) sourceComponent = newSource;
                pageLoader.opacity = 0;
                pageSlide.y = 14;
                _fadeIn.start();
            }

            function getPageComponent() {
                if (_pendingPage === "home") return homePageComponent;
                if (_pendingPage === "search") return searchPageComponent;
                if (_pendingPage === "settings") return settingsPageComponent;
                if (_pendingPage === "new" || _pendingPage === "radio" || _pendingPage === "made")
                    return discoveryPageComponent;
                return libraryPageComponent;
            }

            sourceComponent: homePageComponent

            Component.onCompleted: _fadeIn.start()

            Connections {
                target: root
                function onCurrentPageChanged() {
                    pageLoader.switchPage(root.currentPage);
                }
            }
        }
    }

    // ── Page Components ────────────────────────────────────

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

    Component {
        id: settingsPageComponent
        // New navigable settings shell (rail + detail). Privacy / EULA / About are
        // now sub-routes INSIDE it (AboutLegalPage), not top-level currentPage peers.
        SettingsShell {}
    }

    TransportBar {
        id: transport
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: Theme.transportHeight + 14
        onExpandRequested: nowPlayingView.open = true
        onQueueRequested: playQueueDrawer.open = !playQueueDrawer.open
    }

    // ── Immersive now-playing (full-window, artwork-color background) ──
    NowPlayingView {
        id: nowPlayingView
        objectName: "nowPlayingView"
        anchors.fill: parent
        z: 100
    }

    // ── Play-queue drawer (full-window overlay — must live here, not in the
    //    thin transport bar, to slide in full-height from the right). ──
    PlayQueueDrawer {
        id: playQueueDrawer
        objectName: "playQueueDrawer"
        anchors.fill: parent
        z: 101
    }
}
