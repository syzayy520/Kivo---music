// =============================================================================
// Kivo Music - AppearancePage.qml
// 职责: 外观设置（主题/深色模式）。深色模式经由 Theme 单例全局生效。
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
            text: qsTr("Appearance")
            color: Theme.text
            font.pixelSize: 24
            font.weight: Font.Bold
            font.family: Theme.fontFamily
        }

        SettingsSection {
            title: qsTr("Theme")

            SettingsToggleRow {
                icon: "🌙"
                label: qsTr("Dark Mode")
                sublabel: qsTr("Use a dark color scheme across the whole app")
                checked: Theme.isDark
                onToggled: Theme.isDark = value
            }
        }
    }
}
