// =============================================================================
// Kivo Music - PrivacyPolicy.qml
// 职责: 隐私政策页面（Microsoft Store 审核必需要求）
// =============================================================================

import QtQuick
import QtQuick.Controls
import "../../tokens"

Item {
    id: root

    Theme { id: theme }

    Flickable {
        anchors.fill: parent
        anchors.margins: 40
        contentHeight: policyContent.height + 40
        clip: true

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOn
            contentItem: Rectangle {
                implicitWidth: 6
                radius: 3
                color: "#d0d0d5"
            }
        }

        Column {
            id: policyContent
            width: parent.width - 20
            spacing: 20

            Text {
                text: "Privacy Policy"
                color: theme.text
                font.pixelSize: 28
                font.weight: Font.Bold
            }

            Text {
                text: "Last updated: June 2026"
                color: theme.muted
                font.pixelSize: 13
            }

            Rectangle { width: parent.width; height: 1; color: "#e5e5ea" }

            Text {
                text: "1. Information We Collect"
                color: theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "Kivo Music is a local audio player. We do NOT collect, store, or transmit any personal information to external servers. All your music library data, playlists, and preferences are stored exclusively on your local device.\n\nWe do not require account registration, and we do not track your listening habits."
                color: theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "2. Local Data Storage"
                color: theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "Kivo Music stores the following data locally on your device:\n\n• Music library index (file paths, metadata)\n• Playlists and playback queue\n• Application preferences (theme, volume, playback mode)\n• Cached album artwork\n\nAll data is stored in your local AppData folder and is never shared with third parties."
                color: theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "3. Network Usage"
                color: theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "Kivo Music may make network requests only for the following purposes:\n\n• Fetching album artwork from MusicBrainz Cover Art Archive (when no local cover is found)\n• Fetching synchronized lyrics from online lyric databases (when no local .lrc file is found)\n\nThese requests are made directly from your device and do not pass through any Kivo-controlled servers. No personal data is included in these requests."
                color: theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "4. Third-Party Services"
                color: theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "Kivo Music uses the following third-party libraries:\n\n• FFmpeg (LGPL) — Audio decoding\n• Qt Framework (LGPL) — User interface\n\nThese libraries run entirely on your device and do not transmit data externally."
                color: theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "5. Your Rights"
                color: theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "Since Kivo Music does not collect any personal data, there is no personal data to access, modify, or delete. You can clear all local application data at any time through Windows Settings → Apps → Kivo Music → Advanced Options → Reset."
                color: theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "6. Contact"
                color: theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "If you have questions about this privacy policy, please contact us at:\n\nkivo.music.app@gmail.com"
                color: theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Item { width: 1; height: 40 }
        }
    }
}
