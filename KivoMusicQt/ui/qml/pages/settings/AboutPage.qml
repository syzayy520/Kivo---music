// =============================================================================
// Kivo Music - AboutPage.qml
// 职责: 关于页面（版本号、开发者信息、第三方许可）
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic

Item {
    id: root
    property string appVersion: "1.0.0"


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
            color: Theme.text
            font.pixelSize: 28
            font.weight: Font.Bold
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Version " + root.appVersion
            color: Theme.muted
            font.pixelSize: 14
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 200; height: 1
            color: Theme.line
        }

        // ── Info ──────────────────────────────────────────
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8

            Row {
                spacing: 12
                Text { text: "Developer:"; color: Theme.muted; font.pixelSize: 13; width: 100; horizontalAlignment: Text.AlignRight }
                Text { text: "Kivo Team"; color: Theme.text; font.pixelSize: 13 }
            }
            Row {
                spacing: 12
                Text { text: "Engine:"; color: Theme.muted; font.pixelSize: 13; width: 100; horizontalAlignment: Text.AlignRight }
                Text { text: "Kivo Audio Core 1.0"; color: Theme.text; font.pixelSize: 13 }
            }
            Row {
                spacing: 12
                Text { text: "Framework:"; color: Theme.muted; font.pixelSize: 13; width: 100; horizontalAlignment: Text.AlignRight }
                Text { text: "Qt 6.8.3 + QML"; color: Theme.text; font.pixelSize: 13 }
            }
            Row {
                spacing: 12
                Text { text: "Audio:"; color: Theme.muted; font.pixelSize: 13; width: 100; horizontalAlignment: Text.AlignRight }
                Text { text: "FFmpeg (LGPL)"; color: Theme.text; font.pixelSize: 13 }
            }
            Row {
                spacing: 12
                Text { text: "Build:"; color: Theme.muted; font.pixelSize: 13; width: 100; horizontalAlignment: Text.AlignRight }
                Text { text: "MSVC 2022 x64"; color: Theme.text; font.pixelSize: 13 }
            }
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 200; height: 1
            color: Theme.line
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "© 2026 Kivo. All rights reserved."
            color: Theme.faint
            font.pixelSize: 12
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "This software includes open-source components.\nFFmpeg (LGPL) | Qt Framework (LGPL)"
            color: Theme.faint
            font.pixelSize: 11
            horizontalAlignment: Text.AlignHCenter
            lineHeight: 1.4
        }
    }
}
