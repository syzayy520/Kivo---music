import QtQuick
import QtQuick.Controls
import "."
import "../../tokens"

Button {
    id: root
    property bool current: false
    property string pageKey: ""

    function iconKind() {
        const value = root.text.toLowerCase();
        if (value.indexOf("search") >= 0)
            return "search";
        if (value.indexOf("home") >= 0)
            return "home";
        if (value === "new")
            return "new";
        if (value.indexOf("radio") >= 0)
            return "radio";
        if (value.indexOf("pin") >= 0)
            return "pins";
        if (value.indexOf("recent") >= 0)
            return "recent";
        if (value.indexOf("artist") >= 0)
            return "artist";
        if (value.indexOf("album") >= 0)
            return "album";
        if (value.indexOf("song") >= 0)
            return "song";
        if (value.indexOf("video") >= 0)
            return "video";
        if (value.indexOf("made") >= 0)
            return "made";
        if (value.indexOf("favorite") >= 0)
            return "favorite";
        if (value.indexOf("focus") >= 0)
            return "playlist";
        return "browse";
    }

    height: 32
    flat: true
    leftPadding: 12
    rightPadding: 10
    font.pixelSize: 13
    font.weight: current ? Font.DemiBold : Font.Normal

    Theme { id: theme }

    background: Rectangle {
        radius: 7
        color: root.current ? "#f0e9e8" : (root.hovered ? "#f3eeee" : "transparent")

        // 添加平滑过渡
        Behavior on color {
            ColorAnimation { duration: 150; easing.type: Easing.OutQuad }
        }

        // 微妙的缩放效果
        scale: root.down ? 0.98 : 1.0
        Behavior on scale {
            NumberAnimation { duration: 80; easing.type: Easing.OutQuad }
        }
    }

    contentItem: Row {
        spacing: 10

        NavigationGlyph {
            width: 18
            height: 18
            kind: root.iconKind()
            active: root.current
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: root.text
            color: root.current ? "#fa233b" : theme.text
            font: root.font
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
        }
    }
}
