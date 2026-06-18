import QtQuick

Rectangle {
    id: root
    property int variant: 0
    property real radiusValue: 8

    radius: radiusValue
    clip: true
    color: "#101319"

    // Cohesive, desaturated "after-dark" set — all deep and low-chroma so a wall
    // of fallback covers reads as ONE curated collection, not a rainbow of demo
    // tiles. The canvas glow/silhouette overlay (below) supplies the per-tile
    // variation; the bases stay quiet. (Apple Music's calm comes from restraint.)
    readonly property var palette: [
        ["#202533", "#0c0e15"],  // slate blue
        ["#27222f", "#0f0d14"],  // plum charcoal
        ["#1b2722", "#0c100e"],  // deep forest
        ["#2a2420", "#120f0d"],  // umber
        ["#1f2a31", "#0d1113"],  // steel teal
        ["#2a2129", "#110d12"],  // muted mauve
        ["#1d2433", "#0b0e15"],  // midnight navy
        ["#2b2722", "#121010"],  // warm taupe
        ["#222a2c", "#0d100f"],  // pine
        ["#262030", "#0f0c13"]   // indigo charcoal
    ]

    gradient: Gradient {
        GradientStop { position: 0; color: root.palette[root.variant % root.palette.length][0] }
        GradientStop { position: 1; color: root.palette[root.variant % root.palette.length][1] }
    }

    Canvas {
        anchors.fill: parent
        antialiasing: true
        // P2 修复: 当 root (GeneratedAlbumArtwork) 不可见时, Canvas 仍可能接到 requestPaint。
        // 加 visible 绑定使 Qt Quick 在隐藏时跳过渲染树, 并在 onPaint 开头提前返回。
        visible: root.visible

        onPaint: {
            if (!root.visible) return;
            const ctx = getContext("2d");
            const w = width;
            const h = height;
            const mode = root.variant % 7;
            ctx.clearRect(0, 0, w, h);

            function glow(x, y, r, color) {
                const g = ctx.createRadialGradient(w * x, h * y, 0, w * x, h * y, w * r);
                g.addColorStop(0, color);
                g.addColorStop(0.58, color.replace("0.72", "0.22"));
                g.addColorStop(1, "rgba(255,255,255,0)");
                ctx.fillStyle = g;
                ctx.fillRect(0, 0, w, h);
            }

            function silhouette(cx, base, scale) {
                ctx.fillStyle = "rgba(7,9,14,0.62)";
                ctx.beginPath();
                ctx.arc(w * cx, h * (base - 0.22 * scale), w * 0.085 * scale, 0, Math.PI * 2);
                ctx.fill();
                ctx.beginPath();
                ctx.moveTo(w * (cx - 0.18 * scale), h);
                ctx.quadraticCurveTo(w * cx, h * (base - 0.15 * scale), w * (cx + 0.18 * scale), h);
                ctx.closePath();
                ctx.fill();
            }

            if (mode === 0) {
                glow(0.28, 0.22, 0.34, "rgba(255,218,142,0.72)");
                ctx.fillStyle = "rgba(143,171,190,0.28)";
                ctx.fillRect(0, h * 0.62, w, h * 0.38);
                silhouette(0.58, 0.92, 1.35);
            } else if (mode === 1) {
                glow(0.70, 0.26, 0.52, "rgba(255,114,147,0.72)");
                ctx.fillStyle = "rgba(22,87,120,0.72)";
                ctx.fillRect(0, h * 0.56, w, h * 0.44);
                silhouette(0.50, 0.88, 1.20);
            } else if (mode === 2) {
                glow(0.24, 0.24, 0.22, "rgba(244,213,156,0.54)");
                ctx.fillStyle = "rgba(65,82,70,0.74)";
                ctx.beginPath();
                ctx.moveTo(0, h);
                ctx.lineTo(w * 0.28, h * 0.54);
                ctx.lineTo(w * 0.48, h * 0.74);
                ctx.lineTo(w * 0.76, h * 0.38);
                ctx.lineTo(w, h * 0.66);
                ctx.lineTo(w, h);
                ctx.closePath();
                ctx.fill();
            } else if (mode === 3) {
                glow(0.52, 0.28, 0.30, "rgba(255,205,120,0.50)");
                ctx.fillStyle = "rgba(0,0,0,0.50)";
                ctx.fillRect(0, 0, w, h);
                ctx.fillStyle = "rgba(255,255,255,0.18)";
                ctx.fillRect(w * 0.18, 0, w * 0.035, h);
                ctx.fillRect(w * 0.62, 0, w * 0.025, h);
                silhouette(0.52, 0.94, 1.45);
            } else if (mode === 4) {
                // "Liner notes" — kept dark for collection cohesion: a whisper-light
                // wash with light text bars, instead of the old white page.
                ctx.fillStyle = "rgba(255,255,255,0.05)";
                ctx.fillRect(0, 0, w, h);
                ctx.fillStyle = "rgba(232,234,237,0.34)";
                ctx.fillRect(w * 0.22, h * 0.18, w * 0.54, h * 0.08);
                ctx.fillRect(w * 0.22, h * 0.31, w * 0.38, h * 0.055);
                silhouette(0.50, 0.95, 1.18);
            } else if (mode === 5) {
                glow(0.32, 0.22, 0.22, "rgba(228,213,137,0.56)");
                ctx.strokeStyle = "rgba(180,216,235,0.42)";
                ctx.lineWidth = Math.max(1, w * 0.012);
                ctx.beginPath();
                ctx.moveTo(w * 0.10, h * 0.70);
                ctx.lineTo(w * 0.88, h * 0.52);
                ctx.stroke();
                ctx.fillStyle = "rgba(143,178,193,0.38)";
                ctx.fillRect(0, h * 0.58, w, h * 0.42);
            } else {
                glow(0.24, 0.24, 0.36, "rgba(255,194,112,0.62)");
                ctx.fillStyle = "rgba(20,12,18,0.52)";
                ctx.fillRect(0, 0, w, h);
                ctx.fillStyle = "rgba(255,255,255,0.78)";
                ctx.fillRect(w * 0.13, h * 0.16, w * 0.46, h * 0.045);
                ctx.fillRect(w * 0.13, h * 0.23, w * 0.30, h * 0.03);
            }

            ctx.globalAlpha = 0.10;
            for (let i = 0; i < 26; ++i) {
                ctx.fillStyle = i % 2 ? "#ffffff" : "#000000";
                ctx.fillRect(w * ((i * 37) % 101) / 100, h * ((i * 53) % 97) / 100, w * 0.018, h * 0.012);
            }
            ctx.globalAlpha = 1;
        }
    }
}
