// =============================================================================
// Kivo Music - NavigationItem.qml
// Apple Music-style sidebar navigation item with SF icon, active indicator, ripple
// =============================================================================

import QtQuick
import QtQuick.Controls
import "../../tokens"

Button {
    id: root
    property bool current: false
    property string pageKey: ""

    function iconKind() {
        const v = text.toLowerCase();
        if (v.indexOf("search") >= 0) return "search";
        if (v.indexOf("home") >= 0) return "home";
        if (v === "new") return "new";
        if (v.indexOf("radio") >= 0) return "radio";
        if (v.indexOf("pin") >= 0) return "pins";
        if (v.indexOf("recent") >= 0) return "recent";
        if (v.indexOf("artist") >= 0) return "artist";
        if (v.indexOf("album") >= 0) return "album";
        if (v.indexOf("song") >= 0) return "song";
        if (v.indexOf("video") >= 0) return "video";
        if (v.indexOf("made") >= 0) return "made";
        if (v.indexOf("favorite") >= 0) return "favorite";
        if (v.indexOf("focus") >= 0) return "playlist";
        return "browse";
    }

    height: 34
    flat: true
    leftPadding: 10
    rightPadding: 10
    font.pixelSize: 13
    font.weight: current ? Font.DemiBold : Font.Normal

    Theme { id: theme }

    // ── Background ──────────────────────────────────────────
    background: Rectangle {
        id: bg
        radius: 8
        color: {
            if (root.current) return "#f2e9ea";
            if (root.hovered) return "#f5f2f3";
            return "transparent";
        }
        opacity: root.down ? 0.85 : 1.0

        // Active indicator - left accent bar
        Rectangle {
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.verticalCenter: parent.verticalCenter
            width: 3
            height: root.current ? 18 : 0
            radius: 2
            color: theme.accentPink

            Behavior on height {
                NumberAnimation { duration: 200; easing.type: Easing.OutBack }
            }
        }

        Behavior on color {
            ColorAnimation { duration: 150; easing.type: Easing.OutQuad }
        }

        Behavior on opacity {
            NumberAnimation { duration: 100 }
        }

        scale: root.down ? 0.97 : 1.0
        Behavior on scale {
            NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
        }
    }

    // ── Content ─────────────────────────────────────────────
    contentItem: Row {
        spacing: 10

        NavigationGlyph {
            width: 18
            height: 18
            kind: root.iconKind()
            active: root.current
            anchors.verticalCenter: parent.verticalCenter

            Behavior on scale {
                NumberAnimation { duration: 150; easing.type: Easing.OutBack }
            }
        }

        Text {
            text: root.text
            color: root.current ? theme.accentPink : theme.text
            font: root.font
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight

            Behavior on color {
                ColorAnimation { duration: 150 }
            }
        }
    }

    // ── Tooltip ─────────────────────────────────────────────
    ToolTip {
        visible: root.hovered && !root.current
        text: root.text
        delay: 600
        font.pixelSize: 12
    }
}
