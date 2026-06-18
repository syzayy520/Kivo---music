// =============================================================================
// Kivo Music - glyphs/NextGlyph.qml
// 职责: 只画「下一首」字形(右向三角 + 竖条)。单一职责。
// =============================================================================

import QtQuick

Canvas {
    id: g
    property color glyphColor: "#ffffff"
    onGlyphColorChanged: requestPaint()

    onPaint: {
        const ctx = getContext("2d");
        const w = width;
        const h = height;
        ctx.clearRect(0, 0, w, h);
        ctx.fillStyle = g.glyphColor;
        ctx.fillRect(w * 0.64, h * 0.29, w * 0.06, h * 0.42);
        ctx.beginPath();
        ctx.moveTo(w * 0.30, h * 0.27);
        ctx.lineTo(w * 0.62, h * 0.50);
        ctx.lineTo(w * 0.30, h * 0.73);
        ctx.closePath();
        ctx.fill();
    }
}
