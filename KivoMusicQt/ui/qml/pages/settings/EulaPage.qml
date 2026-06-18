// =============================================================================
// Kivo Music - EulaPage.qml
// 职责: 最终用户许可协议（Microsoft Store 审核必需）
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic

Item {
    id: root


    Flickable {
        anchors.fill: parent
        anchors.margins: 40
        contentHeight: eulaContent.height + 40
        clip: true

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOn
            contentItem: Rectangle {
                implicitWidth: 6; radius: 3; color: Theme.inactive
            }
        }

        Column {
            id: eulaContent
            width: parent.width - 20
            spacing: 20

            Text {
                text: "End User License Agreement"
                color: Theme.text
                font.pixelSize: 28
                font.weight: Font.Bold
            }

            Text {
                text: "Version 1.0 — June 2026"
                color: Theme.muted
                font.pixelSize: 13
            }

            Rectangle { width: parent.width; height: 1; color: Theme.line }

            Text {
                text: "1. License Grant"
                color: Theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "Kivo Music is licensed, not sold. This EULA grants you a non-exclusive, non-transferable license to install and use Kivo Music on your Windows devices for personal, non-commercial use."
                color: Theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "2. Restrictions"
                color: Theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "You may not:\n• Reverse engineer, decompile, or disassemble the software\n• Distribute, sublicense, or transfer the software to third parties\n• Use the software for any illegal purpose\n• Remove or alter any copyright notices"
                color: Theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "3. Ownership"
                color: Theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "Kivo Music and all intellectual property rights therein are owned by the Kivo development team. This software includes open-source components licensed under LGPL and other open-source licenses. See the About page for third-party notices."
                color: Theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "4. Disclaimer of Warranty"
                color: Theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "Kivo Music is provided \"AS IS\" without warranty of any kind. We do not guarantee uninterrupted or error-free operation. You use this software at your own risk."
                color: Theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "5. Limitation of Liability"
                color: Theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "In no event shall the Kivo development team be liable for any damages arising from the use or inability to use this software, including but not limited to data loss, system failure, or any other commercial damages."
                color: Theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "6. Termination"
                color: Theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "This license is effective until terminated. It will terminate automatically if you fail to comply with any term. Upon termination, you must uninstall and destroy all copies of the software."
                color: Theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Text {
                text: "7. Governing Law"
                color: Theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
            }

            Text {
                width: parent.width
                text: "This agreement shall be governed by the laws of the jurisdiction in which the software is distributed."
                color: Theme.muted
                font.pixelSize: 14
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }

            Item { width: 1; height: 40 }
        }
    }
}
