// =============================================================================
// Kivo Music - NowPlayingBackground.qml
// 职责: 沉浸视图的「背景」—— 取色径向光晕 + 模糊封面铺底 + 可读性暗幕。纯展示。
// 超越 AM: 不只是线性渐变,而是一束"从封面散出的有色光",让整屏被音乐点亮。
// =============================================================================

import QtQuick
import QtQuick.Effects
import KivoMusic

Item {
    id: root
    property color baseColor: Qt.rgba(0.16, 0.16, 0.18, 1)
    property url source: ""
    readonly property bool hasSource: source.toString().length > 0

    // z0 — deep base: a near-black field tinted by the cover color so edges feel
    // intentional, not flat black.
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.darker(root.baseColor, 1.7) }
            GradientStop { position: 0.6; color: Qt.darker(root.baseColor, 3.0) }
            GradientStop { position: 1.0; color: Qt.darker(root.baseColor, 5.0) }
        }
    }

    // z1 — extracted-color radial BLOOM, centered where the artwork sits. This is
    // the "colored light" that makes the view feel alive (Apple Music's signature).
    Canvas {
        id: bloom
        anchors.fill: parent
        renderStrategy: Canvas.Cooperative
        property color c: Qt.lighter(root.baseColor, 1.25)
        onCChanged: requestPaint()
        onPaint: {
            const ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            const cx = width * 0.5, cy = height * 0.34, r = Math.max(width, height) * 0.55;
            const g = ctx.createRadialGradient(cx, cy, 0, cx, cy, r);
            g.addColorStop(0.0, Qt.rgba(c.r, c.g, c.b, 0.50));
            g.addColorStop(0.45, Qt.rgba(c.r, c.g, c.b, 0.18));
            g.addColorStop(1.0, Qt.rgba(c.r, c.g, c.b, 0.0));
            ctx.fillStyle = g;
            ctx.fillRect(0, 0, width, height);
        }
        // repaint on resize, debounced so dragging the window stays cheap
        onWidthChanged: _repaint.restart()
        onHeightChanged: _repaint.restart()
        Timer { id: _repaint; interval: 180; onTriggered: bloom.requestPaint() }
    }

    // sourceSize debounce so window drags don't re-decode every frame.
    property size _stableSize: Qt.size(Math.ceil(root.width), Math.ceil(root.height))
    onWidthChanged:  _sizeDebounce.restart()
    onHeightChanged: _sizeDebounce.restart()
    Timer {
        id: _sizeDebounce
        interval: 220
        onTriggered: root._stableSize = Qt.size(Math.ceil(root.width), Math.ceil(root.height))
    }

    // z2 — heavily-blurred full-bleed artwork for depth/texture.
    Image {
        id: artSource
        anchors.fill: parent
        source: root.source
        fillMode: Image.PreserveAspectCrop
        asynchronous: true
        cache: true
        sourceSize: root._stableSize
        visible: false  // texture provider for the blur only
    }
    MultiEffect {
        anchors.fill: parent
        source: artSource
        blurEnabled: true
        blur: 1.0
        blurMax: 64
        autoPaddingEnabled: false
        opacity: root.hasSource && artSource.status === Image.Ready ? 0.45 : 0.0
        visible: opacity > 0.01
        Behavior on opacity { NumberAnimation { duration: 400 } }
    }

    // z3 — legibility scrim: darker toward the bottom where text/controls sit.
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#00000000" }
            GradientStop { position: 0.55; color: Theme.npScrimLight }
            GradientStop { position: 1.0; color: Theme.npScrimMid }
        }
    }
}
