// =============================================================================
// Kivo Music - windows/startup/OnboardingDialog.qml
// 职责: 3步引导对话框，完成后发射 onboardingCompleted 信号。
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import KivoMusic

Dialog {
    id: root

    signal onboardingCompleted

    modal: true
    anchors.centerIn: parent
    width: 480
    height: 400
    padding: 32
    closePolicy: Popup.NoAutoClose

    property int step: 0

    background: Rectangle {
        radius: 14; color: Theme.panel
        border.color: Theme.line; border.width: 1
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true; shadowColor: Theme.shadowModal
            shadowBlur: 0.6; shadowVerticalOffset: 12
        }
    }

    contentItem: Column {
        spacing: 20
        width: parent.width

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.step === 0 ? "🎵" : root.step === 1 ? "📂" : "⌨️"
            font.pixelSize: 56
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.step === 0 ? "Welcome to Kivo Music" :
                  root.step === 1 ? "Your Music Library" : "Power User Ready"
            color: Theme.text
            font.pixelSize: 20
            font.weight: Font.Bold
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.step === 0 ? "A beautiful music player\nfor your local collection." :
                  root.step === 1 ? "Drag & drop audio files, or let Kivo scan\nyour music folders automatically." :
                  "Use keyboard shortcuts to control playback.\nSpace = Play/Pause, ←→ = Seek, Ctrl+M = Mini Player"
            color: Theme.textPlaceholder
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            lineHeight: 1.5
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 6
            Repeater {
                model: 3
                Rectangle {
                    width: 8; height: 8; radius: 4
                    color: root.step >= index ? Theme.ctaPrimary : Theme.inactive
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 12

            Button {
                flat: true
                visible: root.step < 2
                onClicked: root.onboardingCompleted()
                contentItem: Text {
                    text: "Skip"
                    color: Theme.textPlaceholder; font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { radius: 8; color: "transparent" }
            }

            Button {
                onClicked: {
                    if (root.step < 2) root.step++;
                    else root.onboardingCompleted();
                }
                contentItem: Text {
                    text: root.step < 2 ? "Next" : "Get Started"
                    color: Theme.npText; font.pixelSize: 14; font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { radius: 8; color: Theme.ctaPrimary }
            }
        }
    }
}
