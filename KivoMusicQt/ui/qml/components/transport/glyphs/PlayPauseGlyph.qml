// =============================================================================
// Kivo Music - glyphs/PlayPauseGlyph.qml
// 职责: 只画「播放/暂停」字形(三角↔双竖条,可选形变)。单一真源,跨 transport/nowplaying 复用。
// Loading时显示spinner而不是图标。
// =============================================================================

import QtQuick
import "../../common"

Item {
    id: root
    property bool playing: false
    property bool loading: false
    property color glyphColor: "#ffffff"
    property bool morph: true   // true=播放↔暂停平滑形变;false=瞬时切换

    // Show spinner when loading, otherwise show play/pause glyph
    LoadingSpinner {
        anchors.centerIn: parent
        width: parent.width * 0.6
        height: parent.height * 0.6
        color: root.glyphColor
        visible: root.loading
        lineWidth: 2
    }

    Canvas {
        id: g
        anchors.fill: parent
        visible: !root.loading

        property real morphValue: root.playing ? 1.0 : 0.0
        Behavior on morphValue {
            enabled: root.morph
            NumberAnimation { duration: 180; easing.type: Easing.InOutCubic }
        }
        onMorphValueChanged: requestPaint()

        Connections {
            target: root
            function onGlyphColorChanged() { g.requestPaint() }
            function onPlayingChanged() { if (!root.morph) g.requestPaint() }
        }

        onPaint: {
            const ctx = getContext("2d");
            const w = width;
            const h = height;
            const m = root.morph ? g.morphValue : (root.playing ? 1.0 : 0.0);
            ctx.clearRect(0, 0, w, h);
            ctx.fillStyle = root.glyphColor;

            const lx = 0.39 - m * 0.01;
            ctx.fillRect(w * lx, h * 0.29, w * 0.08, h * 0.42);

            if (m < 0.5) {
                ctx.beginPath();
                ctx.moveTo(w * 0.40, h * 0.29);
                ctx.lineTo(w * 0.40, h * 0.71);
                ctx.lineTo(w * (0.74 - m * 0.4), h * 0.50);
                ctx.closePath();
                ctx.fill();
            } else {
                ctx.fillRect(w * 0.56, h * 0.29, w * 0.08, h * 0.42);
            }
        }

        Component.onCompleted: requestPaint()
    }
}
