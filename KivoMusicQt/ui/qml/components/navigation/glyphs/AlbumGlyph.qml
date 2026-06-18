// =============================================================================
// Kivo Music - navigation/glyphs/AlbumGlyph.qml
// 职责: 只画「专辑」字形(方框 + 唱片圈 + 轴心)。单一职责。
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
        ctx.fillStyle = g.glyphColor;
        ctx.lineWidth = g.weight;
        ctx.lineCap = "round";
        ctx.lineJoin = "round";
        ctx.strokeRect(w * 0.23, h * 0.23, w * 0.54, h * 0.54);
        ctx.beginPath();
        ctx.arc(w * 0.50, h * 0.50, w * 0.13, 0, Math.PI * 2);
        ctx.stroke();
        ctx.beginPath();
        ctx.arc(w * 0.50, h * 0.50, w * 0.025, 0, Math.PI * 2);
        ctx.fill();
    }
}
