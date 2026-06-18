// =============================================================================
// Kivo Music - windows/startup/EulaDialog.qml
// 职责: 首次启动 EULA 同意对话框，发射 eulaAccepted / eulaDeclined 信号。
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import KivoMusic

Dialog {
    id: root

    signal eulaAccepted
    signal eulaDeclined

    title: "License Agreement"
    modal: true
    anchors.centerIn: parent
    width: 520
    height: 480
    padding: 28
    closePolicy: Popup.NoAutoClose

    background: Rectangle {
        radius: 14
        color: Theme.panel
        border.color: Theme.line
        border.width: 1
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Theme.shadowModal
            shadowBlur: 0.6
            shadowVerticalOffset: 12
        }
    }

    contentItem: Column {
        spacing: 16
        width: parent.width

        Text {
            text: "📄 License Agreement"
            color: Theme.text
            font.pixelSize: 18
            font.weight: Font.Bold
        }
        Rectangle { width: parent.width; height: 1; color: Theme.line }

        Flickable {
            width: parent.width
            height: 260
            clip: true
            contentHeight: eulaSummary.height

            Text {
                id: eulaSummary
                width: parent.width
                text: "Kivo Music is licensed, not sold. This End User License Agreement (\"EULA\") is a legal agreement between you and the Kivo development team.\n\nBy clicking \"Accept\", you agree to:\n\n• Use this software for personal, non-commercial purposes\n• Not reverse engineer, decompile, or disassemble the software\n• Not distribute or transfer the software to third parties\n\nThis software is provided \"AS IS\" without warranty of any kind.\n\nFull EULA is available in Settings → License Agreement."
                color: Theme.textSecondary
                font.pixelSize: 13
                lineHeight: 1.5
                wrapMode: Text.WordWrap
            }
        }

        Text {
            text: "By continuing, you also acknowledge our Privacy Policy (available in Settings)."
            color: Theme.textPlaceholder
            font.pixelSize: 11
            wrapMode: Text.WordWrap
            width: parent.width
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 12

            Button {
                flat: true
                onClicked: root.eulaDeclined()
                contentItem: Text {
                    text: "Decline"
                    color: Theme.textPlaceholder
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { radius: 8; color: "transparent" }
            }

            Button {
                onClicked: root.eulaAccepted()
                contentItem: Text {
                    text: "Accept"
                    color: Theme.npText
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { radius: 8; color: Theme.ctaPrimary }
            }
        }
    }
}
