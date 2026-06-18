// =============================================================================
// Kivo Music - TransportButton.qml
// 职责: 传输按钮的「外观与交互」(圆形底/悬停/主键金圆/点击),并按 kind 选用 glyphs/
// 家族里对应的字形。本文件不含任何字形几何 —— 几何各自归属 glyphs/ 下的单职责文件。
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import "glyphs"
import KivoMusic

Button {
    id: root
    property bool primary: false
    property string kind: "play"
    property bool loading: false
    // When true (e.g. shuffle/repeat engaged) the glyph tints with the accent.
    property bool active: false
    // Primary (play/pause) = Kivo gold focal point with a dark glyph. Others are
    // theme-aware neutral glyphs (gold only when an action is engaged). Overridable
    // so the immersive now-playing view can force light glyphs on its dark gradient.
    property color glyphColor: primary
        ? Theme.accentInk
        : (active ? Theme.accent : Theme.transportFill)

    width: primary ? 38 : 30
    height: primary ? 38 : 30
    text: ""
    flat: true

    Accessible.role: Accessible.Button
    Accessible.name: {
        switch (root.kind) {
        case "play":    return qsTr("Play")
        case "pause":   return qsTr("Pause")
        case "previous":return qsTr("Previous")
        case "next":    return qsTr("Next")
        case "shuffle": return qsTr("Shuffle")
        case "repeat":  return qsTr("Repeat")
        case "repeat-one": return qsTr("Repeat One")
        case "queue":   return qsTr("Queue")
        case "lyrics":  return qsTr("Lyrics")
        default:        return root.kind
        }
    }
    Accessible.checkable: root.kind === "shuffle" || root.kind === "repeat" || root.kind === "repeat-one"
    Accessible.checked:   root.active


    background: Rectangle {
        radius: width / 2
        color: {
            if (root.primary) return Theme.accent;
            if (root.hovered) return Theme.transportHover;
            return "transparent";
        }
        opacity: root.down ? 0.85 : 1
        border.width: 0

        Behavior on color {
            ColorAnimation { duration: 120; easing.type: Easing.OutQuad }
        }
        Behavior on opacity {
            NumberAnimation { duration: 80 }
        }

        // Soft, restrained shadow for the primary button.
        layer.enabled: root.primary
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#28000000"
            shadowBlur: 0.3
            shadowVerticalOffset: 2
        }
    }

    // 选择对应字形(无几何)。每个 glyph 是 glyphs/ 家族里的单职责文件。
    contentItem: Loader {
        anchors.fill: parent
        sourceComponent: {
            switch (root.kind) {
            case "play":
            case "pause": return cPlayPause;
            case "previous": return cPrevious;
            case "next": return cNext;
            case "shuffle": return cShuffle;
            case "repeat":
            case "repeat-one": return cRepeat;
            case "queue": return cQueue;
            case "lyrics": return cLyrics;
            default: return null;
            }
        }
    }

    Component { id: cPlayPause; PlayPauseGlyph { playing: root.kind === "pause"; glyphColor: root.glyphColor; morph: true; loading: root.loading } }
    Component { id: cPrevious;  PreviousGlyph { glyphColor: root.glyphColor } }
    Component { id: cNext;      NextGlyph { glyphColor: root.glyphColor } }
    Component { id: cShuffle;   ShuffleGlyph { glyphColor: root.glyphColor } }
    Component { id: cRepeat;    RepeatGlyph { glyphColor: root.glyphColor; one: root.kind === "repeat-one" } }
    Component { id: cQueue;     QueueGlyph { glyphColor: root.glyphColor } }
    Component { id: cLyrics;    LyricsGlyph { glyphColor: root.glyphColor } }
}
