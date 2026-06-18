// =============================================================================
// Kivo Music - navigation/glyphs/ArtistGlyph.qml
// 职责: 只画「艺人」字形(头像 + 肩线)。单一职责。
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
        ctx.arc(w * 0.50, h * 0.38, w * 0.14, 0, Math.PI * 2);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(w * 0.25, h * 0.76);
        ctx.quadraticCurveTo(w * 0.50, h * 0.58, w * 0.75, h * 0.76);
        ctx.stroke();
    }
}
