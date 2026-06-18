// =============================================================================
// Kivo Music - glyphs/QueueGlyph.qml
// 职责: 只画「队列」字形(三行列表条)。单一职责。
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
        for (let i = 0; i < 3; ++i) {
            ctx.fillRect(w * 0.25, h * (0.22 + i * 0.26), w * 0.50, h * 0.06);
        }
    }
}
