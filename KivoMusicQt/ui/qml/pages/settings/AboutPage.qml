// =============================================================================
// Kivo Music - AboutPage.qml
// 职责: 关于页面（版本号、开发者信息、第三方许可）
// =============================================================================

import QtQuick
import QtQuick.Controls
import "../../tokens"

Item {
    id: root
    property string appVersion: "1.0.0"

    Theme { id: theme }

    Column {
        anchors.centerIn: parent
        spacing: 24
        width: 400

        // ── Logo ───────────────────────────────────────────
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "🎵"
            font.pixelSize: 64
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Kivo Music"
            color: theme.text
            font.pixelSize: 28
            font.weight: Font.Bold
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Version " + root.appVersion
            color: theme.muted
            font.pixelSize: 14
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 200; height: 1
            color: "#e5e5ea"
        }

        // ── Info ──────────────────────────────────────────
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8

            Row {
                spacing: 12
                Text { text: "Developer:"; color: theme.muted; font.pixelSize: 13; width: 100; horizontalAlignment: Text.AlignRight }
                Text { text: "Kivo Team"; color: theme.text; font.pixelSize: 13 }
            }
            Row {
                spacing: 12
                Text { text: "Engine:"; color: theme.muted; font.pixelSize: 13; width: 100; horizontalAlignment: Text.AlignRight }
                Text { text: "Kivo Audio Core 1.0"; color: theme.text; font.pixelSize: 13 }
            }
            Row {
                spacing: 12
                Text { text: "Framework:"; color: theme.muted; font.pixelSize: 13; width: 100; horizontalAlignment: Text.AlignRight }
                Text { text: "Qt 6.8.3 + QML"; color: theme.text; font.pixelSize: 13 }
            }
            Row {
                spacing: 12
                Text { text: "Audio:"; color: theme.muted; font.pixelSize: 13; width: 100; horizontalAlignment: Text.AlignRight }
                Text { text: "FFmpeg (LGPL)"; color: theme.text; font.pixelSize: 13 }
            }
            Row {
                spacing: 12
                Text { text: "Build:"; color: theme.muted; font.pixelSize: 13; width: 100; horizontalAlignment: Text.AlignRight }
                Text { text: "MSVC 2022 x64"; color: theme.text; font.pixelSize: 13 }
            }
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 200; height: 1
            color: "#e5e5ea"
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "© 2026 Kivo. All rights reserved."
            color: theme.faint
            font.pixelSize: 12
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "This software includes open-source components.\nFFmpeg (LGPL) | Qt Framework (LGPL)"
            color: theme.faint
            font.pixelSize: 11
            horizontalAlignment: Text.AlignHCenter
            lineHeight: 1.4
        }
    }
}
