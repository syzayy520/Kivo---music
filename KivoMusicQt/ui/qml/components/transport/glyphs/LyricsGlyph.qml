// =============================================================================
// Kivo Music - glyphs/LyricsGlyph.qml
// 职责: 只画「歌词」字形(文本行 + 顶部短标)。单一职责。
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
        ctx.strokeStyle = g.glyphColor;
        ctx.fillStyle = g.glyphColor;
        ctx.lineWidth = 1.5;
        ctx.lineCap = "round";
        for (let i = 0; i < 3; ++i) {
            const y = 0.28 + i * 0.22;
            ctx.beginPath();
            ctx.moveTo(w * 0.22, h * y);
            ctx.lineTo(w * 0.78, h * y);
            ctx.stroke();
        }
        ctx.fillRect(w * 0.22, h * 0.22, w * 0.45, h * 0.08);
    }
}
