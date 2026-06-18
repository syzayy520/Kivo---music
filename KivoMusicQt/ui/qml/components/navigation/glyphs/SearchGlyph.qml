// =============================================================================
// Kivo Music - navigation/glyphs/SearchGlyph.qml
// 职责: 只画「搜索」字形(放大镜)。单一职责。
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
        ctx.beginPath();
        ctx.arc(w * 0.44, h * 0.42, w * 0.20, 0, Math.PI * 2);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(w * 0.58, h * 0.58);
        ctx.lineTo(w * 0.76, h * 0.76);
        ctx.stroke();
    }
}
