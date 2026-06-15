// =============================================================================
// Kivo Music - NavigationItem.qml
// Polished Apple Music-style sidebar row
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
        if (v.indexOf("new") >= 0) return "new";
        if (v.indexOf("radio") >= 0) return "radio";
        if (v.indexOf("recent") >= 0) return "recent";
        if (v.indexOf("artist") >= 0) return "artist";
        if (v.indexOf("album") >= 0) return "album";
        if (v.indexOf("song") >= 0) return "song";
        if (v.indexOf("video") >= 0) return "video";
        if (v.indexOf("made") >= 0) return "made";
        if (v.indexOf("favorite") >= 0) return "favorite";
        if (v.indexOf("focus") >= 0) return "playlist";
        if (v.indexOf("setting") >= 0) return "browse";
        return "browse";
    }

    height: 36
    flat: true
    leftPadding: 11
    rightPadding: 10
    font.pixelSize: 13
    font.weight: current ? Font.DemiBold : Font.Normal

    Theme { id: theme }

    background: Rectangle {
        id: bg
        radius: 10
        color: {
            if (root.current) return "#fff0f2";
            if (root.hovered) return "#ffffff";
            return "transparent";
        }
        border.color: root.current ? "#18fa233b" : "transparent"
        border.width: 1
        opacity: root.down ? 0.86 : 1.0

        Rectangle {
            anchors.left: parent.left
            anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
            width: 3
            height: root.current ? 18 : 0
            radius: 2
            color: theme.accent

            Behavior on height {
                NumberAnimation { duration: 170; easing.type: Easing.OutCubic }
            }
        }

        Behavior on color { ColorAnimation { duration: 130 } }
        Behavior on opacity { NumberAnimation { duration: 90 } }
    }

    contentItem: Row {
        spacing: 10

        NavigationGlyph {
            width: 18
            height: 18
            kind: root.iconKind()
            active: root.current
            anchors.verticalCenter: parent.verticalCenter
            opacity: root.current ? 1.0 : 0.78
        }

        Text {
            text: root.text
            color: root.current ? theme.accent : theme.textSecondary
            font: root.font
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
        }
    }

    ToolTip {
        visible: root.hovered && !root.current
        text: root.text
        delay: 700
        font.pixelSize: 12
    }
}
