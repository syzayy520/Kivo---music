// =============================================================================
// Kivo Music - SeekBar.qml
// 职责: 共享的「进度/拖动」基元 —— 轨道 + 已播填充 + 可选把手 + 正确映射的命中区。
// 单一真源:取代 PlaybackControlCluster 与 NowPlayingScrubber 各自手写(且都带过
// -8/÷width 的定位 bug)的两份实现。颜色可配:播放条走主题令牌,沉浸视图走白底暗幕。
// =============================================================================

import QtQuick
import QtQuick.Effects
import KivoMusic

Item {
    id: root
    property real progress: 0                       // 0..1 显示位置
    property color trackColor: Theme.transportTrack
    property color fillColor: Theme.transportFill
    property color handleColor: Theme.npText
    property bool showHandle: true
    property int trackHeight: 3
    signal seekRequested(real position)             // 0..1

    implicitHeight: 14
    height: implicitHeight

    // E5 无障碍支持。注: QtQuick 的 Accessible 附加属性在 6.8 里只有
    // role/name/description/state 等,没有 value/minimumValue/maximumValue/stepSize
    // (那些只在 C++ QAccessibleValueInterface 层),写了会让整棵组件树加载失败。
    Accessible.role: Accessible.Slider
    Accessible.name: qsTr("Playback Progress") + " " + Math.round(root.progress * 100) + "%"
    Accessible.onPressAction: root.seekRequested(root.progress)

    Rectangle {
        id: track
        width: parent.width
        height: (hit.containsMouse || hit.pressed) ? root.trackHeight + 1 : root.trackHeight
        radius: height / 2
        color: root.trackColor
        anchors.verticalCenter: parent.verticalCenter
        Behavior on height { NumberAnimation { duration: 120; easing.type: Easing.OutQuad } }

        Rectangle {
            id: fill
            // 拖动时实时跟随手指(pendingPos),松手后由 progress 接管 —— 比旧实现(松手才动)更跟手。
            width: parent.width * Math.max(0, Math.min(1, hit.pressed ? hit.pendingPos : root.progress))
            height: parent.height
            radius: parent.radius
            color: root.fillColor
            Behavior on width {
                enabled: !hit.pressed
                NumberAnimation { duration: 140; easing.type: Easing.OutQuad }
            }
        }

        Rectangle {
            id: handle
            visible: root.showHandle
            width: 10; height: 10; radius: 5
            color: root.handleColor
            border.color: root.fillColor
            border.width: 1
            x: Math.max(-width / 2, Math.min(fill.width - width / 2, track.width - width / 2))
            anchors.verticalCenter: parent.verticalCenter
            opacity: (hit.containsMouse || hit.pressed) ? 1.0 : 0.0
            scale: hit.pressed ? 1.2 : 1.0
            Behavior on opacity { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
            Behavior on scale { NumberAnimation { duration: 100; easing.type: Easing.OutBack } }
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#26000000"
                shadowBlur: 0.25
                shadowVerticalOffset: 2
            }
        }

        MouseArea {
            id: hit
            anchors.fill: parent
            anchors.margins: -8                     // 放大命中区
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            property real pendingPos: 0
            // 正确映射:命中区比 track 四周各大 8px,本地 x 原点在 track 左侧 8px;
            // 必须减 8 再除以「真实 track 宽度」,否则定位偏 ~2% 且够不到首尾。
            function fractionAt(mx) { return Math.max(0, Math.min(1, (mx - 8) / track.width)) }
            onPressed: function(m) { pendingPos = fractionAt(m.x) }
            onPositionChanged: function(m) { if (pressed) pendingPos = fractionAt(m.x) }
            onReleased: root.seekRequested(pendingPos)
            onClicked: function(m) { root.seekRequested(fractionAt(m.x)) }
        }
    }
}
