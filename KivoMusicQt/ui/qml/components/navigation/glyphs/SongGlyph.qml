// =============================================================================
// Kivo Music - navigation/glyphs/SongGlyph.qml
// 职责: 只画「歌曲」字形(音符)。单一职责。
// =============================================================================

import QtQuick

Canvas {
    id: g
    property color glyphColor: "#6d737d"
    property real weight: 1.4
    onGlyphColorChanged: requestPaint()
    onWeightChanged: requestPaint()

    onPaint: {
        const ctx = getContext("2d");
        const w = width, h = height;
        ctx.clearRect(0, 0, w, h);
        ctx.strokeStyle = g.glyphColor;
        ctx.lineWidth = g.weight;
        ctx.lineCap = "round";
        ctx.lineJoin = "round";
        ctx.beginPath();
        ctx.moveTo(w * 0.38, h * 0.25);
        ctx.lineTo(w * 0.38, h * 0.67);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(w * 0.38, h * 0.27);
        ctx.lineTo(w * 0.70, h * 0.20);
        ctx.lineTo(w * 0.70, h * 0.56);
        ctx.stroke();
        ctx.beginPath();
        ctx.arc(w * 0.31, h * 0.68, w * 0.09, 0, Math.PI * 2);
        ctx.stroke();
        ctx.beginPath();
        ctx.arc(w * 0.63, h * 0.57, w * 0.09, 0, Math.PI * 2);
        ctx.stroke();
    }
}
