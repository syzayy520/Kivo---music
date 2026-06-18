// =============================================================================
// Kivo Music - navigation/glyphs/SettingsGlyph.qml
// 职责: 只画「设置」字形(齿轮)。单一职责。
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
        ctx.arc(w * 0.50, h * 0.50, w * 0.12, 0, Math.PI * 2);
        ctx.stroke();
        for (let i = 0; i < 8; ++i) {
            const a = i * Math.PI / 4;
            ctx.beginPath();
            ctx.moveTo(w * (0.50 + Math.cos(a) * 0.22), h * (0.50 + Math.sin(a) * 0.22));
            ctx.lineTo(w * (0.50 + Math.cos(a) * 0.30), h * (0.50 + Math.sin(a) * 0.30));
            ctx.stroke();
        }
    }
}
