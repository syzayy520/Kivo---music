// =============================================================================
// Kivo Music - glyphs/RepeatGlyph.qml
// 职责: 只画「循环」字形;one=true 时在环中加「1」(单曲循环)。repeat-one 是 repeat 的
// 变体,不是新分支 —— 故一个文件用 one 开关表达,避免复制环形几何。
// =============================================================================

import QtQuick

Canvas {
    id: g
    property color glyphColor: "#ffffff"
    property bool one: false
    onGlyphColorChanged: requestPaint()
    onOneChanged: requestPaint()

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

        ctx.strokeRect(w * 0.27, h * 0.31, w * 0.46, h * 0.35);
        triangle(0.71, 0.23, 0.84, 0.31, 0.71, 0.39);
        triangle(0.29, 0.61, 0.16, 0.69, 0.29, 0.77);

        if (g.one) {
            ctx.lineWidth = 1.8;
            ctx.beginPath();
            ctx.moveTo(w * 0.46, h * 0.45);
            ctx.lineTo(w * 0.50, h * 0.42);
            ctx.lineTo(w * 0.50, h * 0.57);
            ctx.stroke();
        }
    }
}
