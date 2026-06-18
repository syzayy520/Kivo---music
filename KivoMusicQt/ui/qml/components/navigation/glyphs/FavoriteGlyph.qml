// =============================================================================
// Kivo Music - navigation/glyphs/FavoriteGlyph.qml
// 职责: 只画「收藏」字形(心形)。单一职责。
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
        ctx.moveTo(w * 0.50, h * 0.75);
        ctx.bezierCurveTo(w * 0.20, h * 0.55, w * 0.22, h * 0.30, w * 0.42, h * 0.34);
        ctx.bezierCurveTo(w * 0.47, h * 0.35, w * 0.50, h * 0.41, w * 0.50, h * 0.41);
        ctx.bezierCurveTo(w * 0.50, h * 0.41, w * 0.53, h * 0.35, w * 0.58, h * 0.34);
        ctx.bezierCurveTo(w * 0.78, h * 0.30, w * 0.80, h * 0.55, w * 0.50, h * 0.75);
        ctx.stroke();
    }
}
