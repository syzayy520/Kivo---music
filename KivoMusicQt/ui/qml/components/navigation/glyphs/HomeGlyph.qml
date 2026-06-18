// =============================================================================
// Kivo Music - navigation/glyphs/HomeGlyph.qml
// 职责: 只画「首页」字形(房子)。单一职责。
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
        ctx.moveTo(w * 0.24, h * 0.50);
        ctx.lineTo(w * 0.50, h * 0.27);
        ctx.lineTo(w * 0.76, h * 0.50);
        ctx.lineTo(w * 0.76, h * 0.76);
        ctx.lineTo(w * 0.58, h * 0.76);
        ctx.lineTo(w * 0.58, h * 0.58);
        ctx.lineTo(w * 0.42, h * 0.58);
        ctx.lineTo(w * 0.42, h * 0.76);
        ctx.lineTo(w * 0.24, h * 0.76);
        ctx.closePath();
        ctx.stroke();
    }
}
