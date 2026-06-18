// =============================================================================
// Kivo Music - TransportRightCluster.qml
// 职责: 传输条右侧控件组 —— 队列/歌词切换、采样率真相芯片、音量滑块。
// 从 TransportBar 拆出，保持单一职责。
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic
import "../truth"

Row {
    id: root
    spacing: 10

    property real volume: 0.7
    property string truthSampleRate: ""

    signal queueRequested()
    signal lyricsToggled()
    signal volumeRequested(real newVolume)

    // Queue toggle
    TransportButton {
        width: 28; height: 28
        kind: "queue"
        onClicked: root.queueRequested()
    }

    // Lyrics toggle
    TransportButton {
        id: lyricsBtn
        width: 28; height: 28
        kind: "lyrics"
        onClicked: root.lyricsToggled()
    }

    // Bit-perfect sample rate chip + signal-path popover
    Item {
        id: statusArea
        width: statusLabel.visible ? 84 : 0
        height: root.height
        visible: statusLabel.visible
        anchors.verticalCenter: parent.verticalCenter

        Accessible.role: Accessible.Button
        Accessible.name: qsTr("Signal Path")
        Accessible.description: qsTr("View audio signal path and bit-perfect verdict")

        AudioStatusText {
            id: statusLabel
            anchors.fill: parent
            statusText: {
                var rate = root.truthSampleRate
                return /[0-9]/.test(rate) ? rate : ""
            }
            visible: statusText.length > 0
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onClicked: signalPathPopover.opened
                ? signalPathPopover.close()
                : signalPathPopover.open()
            ToolTip.visible: containsMouse && !signalPathPopover.opened
            ToolTip.text: qsTr("View signal path (audio truth)")
        }

        SignalPathPopover {
            id: signalPathPopover
            objectName: "signalPathPopover"
            x: statusArea.width - width
            y: -height - 14
        }
    }

    VolumeCluster {
        volume: root.volume
        anchors.verticalCenter: parent.verticalCenter
        onVolumeRequested: (newVolume) => root.volumeRequested(newVolume)
    }
}
