// =============================================================================
// Kivo Music - navigation/glyphs/RecentGlyph.qml
// 职责: 只画「最近」字形(时钟)。单一职责。
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
        ctx.arc(w * 0.50, h * 0.50, w * 0.27, 0, Math.PI * 2);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(w * 0.50, h * 0.35);
        ctx.lineTo(w * 0.50, h * 0.53);
        ctx.lineTo(w * 0.64, h * 0.61);
        ctx.stroke();
    }
}
