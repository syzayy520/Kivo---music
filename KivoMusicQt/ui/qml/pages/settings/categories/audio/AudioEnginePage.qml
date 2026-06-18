// =============================================================================
// Kivo Music - AudioEnginePage.qml
// 职责: 音频引擎设置（输出设备/独占/位完美）。设备选择待引擎 ABI 暴露枚举后启用。
// =============================================================================

import QtQuick
import KivoMusic

Flickable {
    id: page
    anchors.fill: parent
    contentWidth: width
    contentHeight: col.implicitHeight + 2 * Theme.space6
    clip: true

    Column {
        id: col
        x: Theme.space6
        y: Theme.space6
        width: page.width - 2 * Theme.space6
        spacing: Theme.space6

        Text {
            text: qsTr("Audio Engine")
            color: Theme.text
            font.pixelSize: 24
            font.weight: Font.Bold
            font.family: Theme.fontFamily
        }

        SettingsSection {
            title: qsTr("Output")

            // Output-device selection is gated until the audio-core C ABI exposes
            // device enumeration (a backend task) — shown as a clear placeholder
            // rather than the old dead "Default" label.
            SettingsValueRow {
                icon: "🔊"
                label: qsTr("Output Device")
                value: qsTr("System Default")
                available: false
                note: qsTr("Device selection arrives with a future audio-engine update")
            }
        }
    }
}
