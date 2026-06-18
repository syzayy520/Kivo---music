// =============================================================================
// Kivo Music - WaveformBar.qml
// 职责: 单个频谱柱——独立可复用的可视化组件
// Apple Music 风格: 圆角柱 + 高度动画 + 播放状态跟随
// =============================================================================

import QtQuick
import KivoMusic

Rectangle {
    id: bar

    // ── 公共属性 ────────────────────────────────────────────
    property real targetHeight: 8     // 目标高度（由父组件驱动）
    property color barColor: Theme.accent
    property bool active: true        // 是否激活动画

    width: 3
    height: targetHeight
    radius: 1.5
    color: barColor
    opacity: active ? 0.8 : 0.2

    anchors.bottom: parent ? parent.bottom : undefined

    // ── 平滑高度过渡 ────────────────────────────────────────
    Behavior on height {
        SmoothedAnimation {
            duration: 200 + Math.random() * 200
            velocity: -1
        }
    }

    Behavior on opacity {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutQuad
        }
    }
}
