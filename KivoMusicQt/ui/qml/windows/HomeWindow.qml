import QtQuick
import "../tokens"
import "../components/navigation"
import "../components/shell"
import "../components/transport"
import "../animations"
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
        if (currentPage === "home") return "Home";
        if (currentPage === "artists") return "Artists";
        if (currentPage === "albums") return "Albums";
        if (currentPage === "songs") return "Songs";
        if (currentPage === "recent") return "Recently Added";
        if (currentPage === "new") return "New Music";
        if (currentPage === "made") return "Made for You";
        if (currentPage === "search") return "Search";
        if (currentPage === "settings") return "Settings";
        if (currentPage === "privacy") return "Privacy Policy";
        if (currentPage === "eula") return "License Agreement";
        if (currentPage === "about") return "About Kivo Music";
        return "Library";
    }

    Theme { id: theme }

    Rectangle {
        anchors.fill: parent
        color: theme.page
    }

    Rectangle {
        anchors.left: sidebar.right
        anchors.right: parent.right
        anchors.top: parent.top
        height: 260
        opacity: 0.78
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#ffffff" }
            GradientStop { position: 1.0; color: "#00ffffff" }
        }
    }

    FileDropArea {
        id: fileDropArea
        audioController: audioController
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
            height: 74
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
            asynchronous: true

            property string _pendingPage: ""
            property var _fadeOutAnim: null
            property var _fadeInAnim: null

            function switchPage(pageKey) {
                if (_pendingPage !== "" || (sourceComponent && opacity < 1)) {
                    return;
                }
                _pendingPage = pageKey;
                var targetY = y;
                opacity = 1;
                y = targetY;

                var outAnim = Qt.createQmlObject(
                    'import QtQuick; SequentialAnimation { ' +
                    '  ParallelAnimation { ' +
                    '    NumberAnimation { target: pageLoader; property: "opacity"; from: 1; to: 0; duration: 120; easing.type: Easing.InQuad } ' +
                    '    NumberAnimation { target: pageLoader; property: "y"; from: ' + targetY + '; to: ' + (targetY + 10) + '; duration: 120; easing.type: Easing.InQuad } ' +
                    '  } ' +
                    '  ScriptAction { script: pageLoader._onFadeOutDone(); } ' +
                    '}',
                    pageLoader
                );
                _fadeOutAnim = outAnim;
                outAnim.start();
            }

            function _onFadeOutDone() {
                _fadeOutAnim = null;
                var newSource = getPageComponent();
                if (newSource !== sourceComponent) {
                    sourceComponent = newSource;
                }

                var targetY = y;
                y = targetY + 10;
                opacity = 0;

                var inAnim = Qt.createQmlObject(
                    'import QtQuick; SequentialAnimation { ' +
                    '  ParallelAnimation { ' +
                    '    NumberAnimation { target: pageLoader; property: "opacity"; from: 0; to: 1; duration: 220; easing.type: Easing.OutCubic } ' +
                    '    NumberAnimation { target: pageLoader; property: "y"; from: ' + (targetY + 10) + '; to: ' + targetY + '; duration: 220; easing.type: Easing.OutCubic } ' +
                    '  } ' +
                    '  ScriptAction { script: pageLoader._onFadeInDone(); } ' +
                    '}',
                    pageLoader
                );
                _fadeInAnim = inAnim;
                inAnim.start();
            }

            function _onFadeInDone() {
                _fadeInAnim = null;
                _pendingPage = "";
                y = anchors.topMargin || 0;
            }

            function getPageComponent() {
                if (_pendingPage === "home") return homePageComponent;
                if (_pendingPage === "search") return searchPageComponent;
                if (_pendingPage === "settings") return settingsPageComponent;
                if (_pendingPage === "privacy") return privacyPageComponent;
                if (_pendingPage === "eula") return eulaPageComponent;
                if (_pendingPage === "about") return aboutPageComponent;
                if (_pendingPage === "new" || _pendingPage === "radio" || _pendingPage === "made")
                    return discoveryPageComponent;
                return libraryPageComponent;
            }

            sourceComponent: homePageComponent

            Component.onCompleted: {
                opacity = 0;
                y = y + 10;
                var initAnim = Qt.createQmlObject(
                    'import QtQuick; ParallelAnimation { ' +
                    '  NumberAnimation { target: pageLoader; property: "opacity"; from: 0; to: 1; duration: 260; easing.type: Easing.OutCubic } ' +
                    '  NumberAnimation { target: pageLoader; property: "y"; from: ' + y + '; to: ' + (y - 10) + '; duration: 260; easing.type: Easing.OutCubic } ' +
                    '}',
                    pageLoader
                );
                initAnim.start();
            }

            Connections {
                target: root
                function onCurrentPageChanged() {
                    pageLoader.switchPage(root.currentPage);
                }
            }
        }
    }

    Component { id: homePageComponent; HomePage {} }
    Component { id: libraryPageComponent; LibraryPage { mode: root.currentPage } }
    Component { id: searchPageComponent; SearchPage {} }
    Component { id: discoveryPageComponent; DiscoveryPage { mode: root.currentPage } }
    Component {
        id: settingsPageComponent
        SettingsPage {
            onShowPrivacyPolicy: root.currentPage = "privacy"
            onShowEula: root.currentPage = "eula"
            onShowAbout: root.currentPage = "about"
        }
    }
    Component { id: privacyPageComponent; PrivacyPolicy {} }
    Component { id: eulaPageComponent; EulaPage {} }
    Component { id: aboutPageComponent; AboutPage {} }

    TransportBar {
        id: transport
        anchors.left: sidebar.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: theme.transportHeight + 44
    }
}
