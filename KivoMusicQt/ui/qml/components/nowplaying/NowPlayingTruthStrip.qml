// =============================================================================
// Kivo Music - NowPlayingTruthStrip.qml
// 职责: 沉浸视图的「信号链真相」胶囊 —— 格式 · 采样率 · 位深 + bit-perfect 徽章。
// 这是超越 Apple Music 的差异化:发烧友想看的保真度证据,AM 根本不显示。这里把它
// 从一行灰字提升为一枚有信心的元件。证据门控:无真相数据时绝不虚标。
// =============================================================================

import QtQuick
import KivoMusic

Item {
    id: root
    implicitWidth: capsule.width
    implicitHeight: 36

    // Tapped to expand the full signal-path chain in the immersive view.
    signal clicked()

    // Evidence flags (mirror the C++ truth getters; all default to not-shown).
    readonly property bool hasFormat: {
        const f = audioController.formatText
        return f.length > 0 && f !== "Ready" && f !== "Playing" && f !== "Unknown"
    }
    readonly property bool hasRate: /[0-9]/.test(audioController.truthSampleRate)
    readonly property bool hasDepth: /[0-9]/.test(audioController.truthBitDepth)
    readonly property bool bitPerfect: audioController.truthBitPerfectConfirmed

    // Frosted capsule sized to its content.
    Rectangle {
        id: capsule
        height: parent.height
        width: row.width + 32
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        radius: height / 2
        // Brightens on hover to signal it's tappable (→ expands the full chain).
        color: capsuleHover.containsMouse ? Theme.npControlHover : Theme.npControlBg
        border.color: Theme.npSeparator   // 40% white hairline
        border.width: 1
        Behavior on color { ColorAnimation { duration: Theme.animFast } }

        Row {
            id: row
            anchors.centerIn: parent
            spacing: 9

            // Lossless lead-glyph — a small diamond, gold when bit-perfect confirmed,
            // soft white otherwise. Quietly signals "this is the real signal path".
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "◆"
                font.pixelSize: 9
                color: root.bitPerfect ? Theme.accent : Theme.npTextFaint
            }

            Text {
                visible: root.hasFormat
                anchors.verticalCenter: parent.verticalCenter
                text: audioController.formatText
                color: Theme.npText
                font.pixelSize: 12
                font.weight: Font.DemiBold
                font.letterSpacing: 0.4
                font.family: Theme.fontFamily
            }
            Text {
                visible: root.hasFormat && (root.hasRate || root.hasDepth)
                anchors.verticalCenter: parent.verticalCenter
                text: "·"; color: Theme.npSeparator; font.pixelSize: 12
            }
            Text {
                visible: root.hasRate
                anchors.verticalCenter: parent.verticalCenter
                text: audioController.truthSampleRate
                color: Theme.npTextMuted
                font.pixelSize: 12
                font.family: Theme.fontFamily
            }
            Text {
                visible: root.hasRate && root.hasDepth
                anchors.verticalCenter: parent.verticalCenter
                text: "·"; color: Theme.npSeparator; font.pixelSize: 12
            }
            Text {
                visible: root.hasDepth
                anchors.verticalCenter: parent.verticalCenter
                text: audioController.truthBitDepth
                color: Theme.npTextMuted
                font.pixelSize: 12
                font.family: Theme.fontFamily
            }

            // Bit-perfect badge — gold, evidence-gated. Only when TRULY confirmed.
            Rectangle {
                visible: root.bitPerfect
                anchors.verticalCenter: parent.verticalCenter
                height: 20
                width: bpRow.width + 16
                radius: 10
                color: Theme.accent

                Row {
                    id: bpRow
                    anchors.centerIn: parent
                    spacing: 4
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "✓"; color: Theme.accentInk
                        font.pixelSize: 11; font.weight: Font.Bold
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("Bit-Perfect")
                        color: Theme.accentInk
                        font.pixelSize: 11
                        font.weight: Font.Bold
                        font.letterSpacing: 0.3
                        font.family: Theme.fontFamily
                    }
                }
            }

            // Expand affordance — hints the full signal-path chain is one tap away.
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "›"
                color: Theme.npTextFaint
                font.pixelSize: 15
                font.family: Theme.fontFamily
            }
        }

        MouseArea {
            id: capsuleHover
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.clicked()
        }
    }
}
