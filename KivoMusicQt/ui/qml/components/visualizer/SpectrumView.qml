// =============================================================================
// Kivo Music - SpectrumView.qml
// 职责: 频谱可视化 —— 24 频段 WaveformBar 阵列，播放时呈现 Apple Music 风格跳动。
// 当前版本: 30fps 伪随机驱动(审美驱动), 非真实 FFT 数据。
// E9: 当 C++ SpectrumAnalyzer 就绪后，将 _heights 绑定替换为 analyzerBands。
// =============================================================================

import QtQuick
import KivoMusic

Item {
    id: root
    property bool playing: false

    implicitWidth: 24 * 4 - 1  // 24 bars × (3px + 1px gap) − trailing gap
    implicitHeight: 28

    // ── Spring-physics bar heights (drives WaveformBar.targetHeight) ─────────
    // Index 0 = bass (leftmost), index 23 = treble.
    property var _h: [3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3]
    property var _v: [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

    // 30 fps timer — only runs when playing.
    Timer {
        interval: 33
        running: root.playing
        repeat: true
        onTriggered: _tick()
    }

    // Reset bars to minimum when playback stops.
    onPlayingChanged: {
        if (!playing) {
            _h = [3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3]
            _v = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        }
    }

    function _tick() {
        var h = _h.slice()
        var v = _v.slice()
        var maxH = implicitHeight - 2

        for (var i = 0; i < 24; i++) {
            // Envelope: shaped like a music spectrum — stronger mids, rolled-off extremes.
            // Sin(π*freq) peaks at 0.5 (midrange). Add spike noise.
            var freq = i / 23.0
            var envelope = (Math.sin(freq * Math.PI) * 0.8 + 0.2)
            var spike = Math.random() < 0.12 ? Math.random() * maxH * 0.7 : 0
            var target = envelope * maxH * (0.35 + Math.random() * 0.55) + spike

            // Fast attack (spring stiffness 0.6), slow decay (stiffness 0.12).
            // This matches the feel of a hardware VU meter.
            var stiff = target > h[i] ? 0.55 : 0.12
            var damp  = 0.65
            var acc   = (target - h[i]) * stiff - v[i] * damp
            v[i] = v[i] + acc
            h[i] = Math.max(3, Math.min(maxH, h[i] + v[i]))
        }

        _h = h
        _v = v
    }

    // ── Bar array (bottom-anchored, grows upward) ────────────────────────────
    Item {
        anchors.fill: parent
        Repeater {
            model: 24
            Item {
                x: index * 4   // 3px bar + 1px gap
                width: 3
                height: root.implicitHeight

                WaveformBar {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    targetHeight: root._h[index] || 3
                    // Gradient: bass = accent gold, treble = softer tint.
                    barColor: Qt.tint(
                        Theme.accent,
                        Qt.rgba(1, 1, 1, index / 46.0))
                    active: root.playing
                }
            }
        }
    }
}
