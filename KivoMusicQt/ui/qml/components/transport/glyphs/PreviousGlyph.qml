// =============================================================================
// Kivo Music - glyphs/PreviousGlyph.qml
// 职责: 只画「上一首」字形(竖条 + 左向三角)。单一职责。
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
        ctx.fillRect(w * 0.30, h * 0.29, w * 0.06, h * 0.42);
        ctx.beginPath();
        ctx.moveTo(w * 0.70, h * 0.27);
        ctx.lineTo(w * 0.38, h * 0.50);
        ctx.lineTo(w * 0.70, h * 0.73);
        ctx.closePath();
        ctx.fill();
    }
}
