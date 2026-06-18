// =============================================================================
// Kivo Music - SignalPathStage.qml
// 职责: 信号链上的「一个节点」—— 圆点 + 向下连接线 + 阶段名 + 证据值。单一职责。
// state: 1=完好(金/直通) · -1=被破坏(警示) · 0=未知(中性灰)。颜色即真相。
// =============================================================================

import QtQuick
import KivoMusic

Item {
    id: stage
    property string title: ""
    property string value: ""
    property int state: 0       // 1 intact · -1 broken · 0 unknown
    property bool isLast: false

    width: parent ? parent.width : 260
    implicitHeight: 48

    // "not bit-perfect" is caution, not an error — amber, never alarming red.
    readonly property color _caution: "#e0913f"
    readonly property color dotColor: state === 1 ? Theme.accent
        : state === -1 ? _caution : Theme.faint
    readonly property color lineColor: state === 1 ? Theme.accent : Theme.line

    // Continuous connector down THROUGH the next node (from this dot's centre to
    // the next dot's centre, so the spine reads as one unbroken line).
    Rectangle {
        visible: !stage.isLast
        x: 5; width: 2
        y: 11
        height: stage.height
        radius: 1
        color: stage.lineColor
        opacity: stage.state === 1 ? 0.85 : 0.4
    }

    // The node dot.
    Rectangle {
        id: dot
        x: 0; y: 5
        width: 12; height: 12; radius: 6
        color: stage.dotColor
        Rectangle {  // inner pip — only on an intact (verified) node
            anchors.centerIn: parent
            width: 4; height: 4; radius: 2
            color: stage.state === 1 ? Theme.accentInk : "transparent"
        }
    }

    Column {
        anchors.left: parent.left
        anchors.leftMargin: 26
        anchors.right: parent.right
        spacing: 2

        Text {
            text: stage.title
            color: Theme.muted
            font.pixelSize: 10
            font.weight: Font.DemiBold
            font.letterSpacing: 0.8
            font.capitalization: Font.AllUppercase
            font.family: Theme.fontFamily
        }
        Text {
            width: parent.width
            text: stage.value
            color: stage.state === 0 ? Theme.faint : Theme.text
            font.pixelSize: 13
            font.weight: Font.DemiBold
            font.family: Theme.fontFamily
            elide: Text.ElideRight
        }
    }
}
