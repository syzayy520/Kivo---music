// =============================================================================
// Kivo Music - DetailTrackRow.qml
// 职责: 详情页曲目列表里的「一行」—— 序号/悬停播放、标题、专辑。单一职责。
// 正在播放的曲目以金色高亮(与队列行同一套语言)。
// =============================================================================

import QtQuick
import KivoMusic

Rectangle {
    id: root
    property int trackNumber: 0
    property string title: ""
    property string duration: ""  // e.g. "3:45"
    property bool current: false
    signal activated()

    height: 48
    radius: Theme.radiusMedium
    color: hover.containsMouse ? Theme.panelHover : "transparent"
    Behavior on color { ColorAnimation { duration: 120 } }

    // ── Lead: track number, swaps to a play glyph on hover ───────────────────
    Item {
        id: lead
        width: 40; height: parent.height
        anchors.left: parent.left

        Text {
            anchors.centerIn: parent
            visible: !hover.containsMouse && !root.current
            text: root.trackNumber
            color: Theme.faint
            font.pixelSize: 13; font.family: Theme.fontFamily
        }
        Text {
            anchors.centerIn: parent
            visible: root.current && !hover.containsMouse
            text: "♪"; color: Theme.accent; font.pixelSize: 14
        }
        Canvas {
            anchors.centerIn: parent
            width: 12; height: 12
            visible: hover.containsMouse
            onPaint: {
                const c = getContext("2d"); c.clearRect(0, 0, 12, 12);
                c.fillStyle = Theme.text;
                c.beginPath(); c.moveTo(2, 1); c.lineTo(2, 11); c.lineTo(11, 6);
                c.closePath(); c.fill();
            }
        }
    }

    Text {
        id: titleText
        anchors.left: lead.right
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - lead.width - durationText.width - 60
        text: root.title
        color: root.current ? Theme.accentText : Theme.text
        font.pixelSize: 14
        font.weight: root.current ? Font.DemiBold : Font.Normal
        font.family: Theme.fontFamily
        elide: Text.ElideRight
    }

    Text {
        id: durationText
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        text: root.duration
        color: Theme.muted
        font.pixelSize: 13
        font.family: Theme.monoFamily
    }

    MouseArea {
        id: hover
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.activated()
    }
}
