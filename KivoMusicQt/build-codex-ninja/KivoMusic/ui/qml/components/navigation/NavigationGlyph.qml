import QtQuick
import "../../tokens"

Canvas {
    id: root
    property string kind: "browse"
    property int variant: 0
    property bool active: false

    Theme { id: theme }

    onKindChanged: requestPaint()
    onVariantChanged: requestPaint()
    onActiveChanged: requestPaint()
    onPaint: {
        const ctx = getContext("2d");
        const w = width;
        const h = height;
        const c = active ? theme.ink : "#6d737d";
        ctx.clearRect(0, 0, w, h);
        ctx.strokeStyle = c;
        ctx.fillStyle = c;
        ctx.lineWidth = active ? 1.55 : 1.35;
        ctx.lineCap = "round";
        ctx.lineJoin = "round";

        function circle(x, y, r, fill) {
            ctx.beginPath();
            ctx.arc(w * x, h * y, w * r, 0, Math.PI * 2);
            if (fill)
                ctx.fill();
            else
                ctx.stroke();
        }

        function line(x1, y1, x2, y2) {
            ctx.beginPath();
            ctx.moveTo(w * x1, h * y1);
            ctx.lineTo(w * x2, h * y2);
            ctx.stroke();
        }

        if (kind === "search") {
            circle(0.44, 0.42, 0.20, false);
            line(0.58, 0.58, 0.76, 0.76);
        } else if (kind === "home") {
            ctx.beginPath();
            ctx.moveTo(w * 0.24, h * 0.50);
            ctx.lineTo(w * 0.50, h * 0.27);
            ctx.lineTo(w * 0.76, h * 0.50);
            ctx.lineTo(w * 0.76, h * 0.76);
            ctx.lineTo(w * 0.58, h * 0.76);
            ctx.lineTo(w * 0.58, h * 0.58);
            ctx.lineTo(w * 0.42, h * 0.58);
            ctx.lineTo(w * 0.42, h * 0.76);
            ctx.lineTo(w * 0.24, h * 0.76);
            ctx.closePath();
            ctx.stroke();
        } else if (kind === "browse") {
            for (let y = 0; y < 2; ++y) {
                for (let x = 0; x < 2; ++x) {
                    ctx.strokeRect(w * (0.25 + x * 0.28), h * (0.25 + y * 0.28), w * 0.18, h * 0.18);
                }
            }
        } else if (kind === "new") {
            for (let y = 0; y < 2; ++y) {
                for (let x = 0; x < 2; ++x) {
                    ctx.strokeRect(w * (0.25 + x * 0.28), h * (0.25 + y * 0.28), w * 0.18, h * 0.18);
                }
            }
            ctx.fillStyle = "#fa233b";
            ctx.fillRect(w * 0.45, h * 0.22, w * 0.10, h * 0.56);
            ctx.fillRect(w * 0.22, h * 0.45, w * 0.56, h * 0.10);
        } else if (kind === "radio") {
            circle(0.50, 0.52, 0.23, false);
            circle(0.50, 0.52, 0.055, true);
            line(0.50, 0.29, 0.66, 0.16);
        } else if (kind === "album") {
            ctx.strokeRect(w * 0.23, h * 0.23, w * 0.54, h * 0.54);
            circle(0.50, 0.50, 0.13, false);
            circle(0.50, 0.50, 0.025, true);
        } else if (kind === "artist") {
            circle(0.50, 0.38, 0.14, false);
            ctx.beginPath();
            ctx.moveTo(w * 0.25, h * 0.76);
            ctx.quadraticCurveTo(w * 0.50, h * 0.58, w * 0.75, h * 0.76);
            ctx.stroke();
        } else if (kind === "pins") {
            ctx.beginPath();
            ctx.moveTo(w * 0.50, h * 0.22);
            ctx.quadraticCurveTo(w * 0.68, h * 0.34, w * 0.56, h * 0.52);
            ctx.lineTo(w * 0.70, h * 0.72);
            ctx.lineTo(w * 0.30, h * 0.72);
            ctx.lineTo(w * 0.44, h * 0.52);
            ctx.quadraticCurveTo(w * 0.32, h * 0.34, w * 0.50, h * 0.22);
            ctx.stroke();
            line(0.50, 0.72, 0.50, 0.86);
        } else if (kind === "song") {
            line(0.38, 0.25, 0.38, 0.67);
            line(0.38, 0.27, 0.70, 0.20);
            line(0.70, 0.20, 0.70, 0.56);
            circle(0.31, 0.68, 0.09, false);
            circle(0.63, 0.57, 0.09, false);
        } else if (kind === "playlist") {
            line(0.25, 0.32, 0.67, 0.32);
            line(0.25, 0.50, 0.67, 0.50);
            line(0.25, 0.68, 0.56, 0.68);
            circle(0.77, 0.68, 0.08, false);
        } else if (kind === "video") {
            ctx.strokeRect(w * 0.22, h * 0.30, w * 0.44, h * 0.40);
            ctx.beginPath();
            ctx.moveTo(w * 0.66, h * 0.42);
            ctx.lineTo(w * 0.80, h * 0.34);
            ctx.lineTo(w * 0.80, h * 0.66);
            ctx.lineTo(w * 0.66, h * 0.58);
            ctx.stroke();
        } else if (kind === "made") {
            ctx.strokeRect(w * 0.27, h * 0.28, w * 0.46, h * 0.46);
            circle(0.50, 0.50, 0.12, false);
            ctx.beginPath();
            ctx.moveTo(w * 0.36, h * 0.28);
            ctx.quadraticCurveTo(w * 0.50, h * 0.16, w * 0.64, h * 0.28);
            ctx.stroke();
        } else if (kind === "recent") {
            circle(0.50, 0.50, 0.27, false);
            line(0.50, 0.35, 0.50, 0.53);
            line(0.50, 0.53, 0.64, 0.61);
        } else if (kind === "favorite") {
            ctx.beginPath();
            ctx.moveTo(w * 0.50, h * 0.75);
            ctx.bezierCurveTo(w * 0.20, h * 0.55, w * 0.22, h * 0.30, w * 0.42, h * 0.34);
            ctx.bezierCurveTo(w * 0.47, h * 0.35, w * 0.50, h * 0.41, w * 0.50, h * 0.41);
            ctx.bezierCurveTo(w * 0.50, h * 0.41, w * 0.53, h * 0.35, w * 0.58, h * 0.34);
            ctx.bezierCurveTo(w * 0.78, h * 0.30, w * 0.80, h * 0.55, w * 0.50, h * 0.75);
            ctx.stroke();
        } else if (kind === "import") {
            ctx.strokeRect(w * 0.25, h * 0.50, w * 0.50, h * 0.24);
            line(0.50, 0.23, 0.50, 0.58);
            line(0.36, 0.44, 0.50, 0.58);
            line(0.64, 0.44, 0.50, 0.58);
        } else if (kind === "settings") {
            circle(0.50, 0.50, 0.12, false);
            for (let i = 0; i < 8; ++i) {
                const angle = i * Math.PI / 4;
                const x1 = 0.50 + Math.cos(angle) * 0.22;
                const y1 = 0.50 + Math.sin(angle) * 0.22;
                const x2 = 0.50 + Math.cos(angle) * 0.30;
                const y2 = 0.50 + Math.sin(angle) * 0.30;
                line(x1, y1, x2, y2);
            }
        } else {
            line(0.28, 0.34, 0.72, 0.34);
            line(0.28, 0.50, 0.72, 0.50);
            line(0.28, 0.66, 0.72, 0.66);
        }
    }
}
