// =============================================================================
// Kivo Music - glyphs/ShuffleGlyph.qml
// 职责: 只画「随机」字形(两条交叉箭头)。单一职责。
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
        ctx.lineWidth = 1.6;
        ctx.lineCap = "round";
        ctx.lineJoin = "round";

        function triangle(x1, y1, x2, y2, x3, y3) {
            ctx.beginPath();
            ctx.moveTo(w * x1, h * y1);
            ctx.lineTo(w * x2, h * y2);
            ctx.lineTo(w * x3, h * y3);
            ctx.closePath();
            ctx.fill();
        }

        ctx.beginPath();
        ctx.moveTo(w * 0.24, h * 0.34);
        ctx.lineTo(w * 0.42, h * 0.34);
        ctx.quadraticCurveTo(w * 0.56, h * 0.34, w * 0.66, h * 0.22);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(w * 0.24, h * 0.66);
        ctx.lineTo(w * 0.42, h * 0.66);
        ctx.quadraticCurveTo(w * 0.56, h * 0.66, w * 0.66, h * 0.78);
        ctx.stroke();
        triangle(0.67, 0.16, 0.82, 0.22, 0.67, 0.29);
        triangle(0.67, 0.71, 0.82, 0.78, 0.67, 0.84);
    }
}
