// =============================================================================
// Kivo Music - App.qml
// Apple Music-level root window with EULA consent, onboarding, and Store compliance
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtCore
import KivoMusic

ApplicationWindow {
    id: window
    property alias currentPage: shell.currentPage

    width: 1440
    height: 900
    minimumWidth: 1020
    minimumHeight: 680
    visible: true
    title: "Kivo Music"
    color: "#f5f5f7"
    flags: Qt.Window | Qt.FramelessWindowHint

    // ── Persisted settings ──────────────────────────────────────
    Settings {
        id: appSettings
        property bool eulaAccepted: false
        property bool onboardingDone: false
        property string appVersion: "1.0.0"
        category: "KivoMusic"
    }

    // ── Global Keyboard Shortcuts ──────────────────────────────
    KeyboardShortcuts {
        id: keyboardShortcuts
        window: window
        audioController: audioController
    }

    // ── Startup Animation ──────────────────────────────────────
    Rectangle {
        id: splashOverlay
        anchors.fill: parent
        color: "#f5f5f7"
        z: 9999
        opacity: 1

        Column {
            anchors.centerIn: parent
            spacing: 16

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "🎵"
                font.pixelSize: 48
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Kivo Music"
                color: "#1d1d1f"
                font.pixelSize: 28
                font.weight: Font.Bold
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Your music, elevated."
                color: "#aeaeb2"
                font.pixelSize: 14
            }
        }

        ParallelAnimation {
            id: splashFade
            running: true
            ScriptAction { script: { splashTimer.start() } }
        }
    }

    Timer {
        id: splashTimer
        interval: 800
        onTriggered: { splashFadeOut.start() }
    }

    SequentialAnimation {
        id: splashFadeOut
        NumberAnimation {
            target: splashOverlay
            property: "opacity"
            to: 0
            duration: 400
            easing.type: Easing.OutQuad
        }
        ScriptAction {
            script: {
                splashOverlay.visible = false;
                checkFirstRun();
            }
        }
    }

    // ── First-run check ─────────────────────────────────────────
    function checkFirstRun() {
        if (!appSettings.eulaAccepted) {
            eulaDialog.open();
        } else if (!appSettings.onboardingDone) {
            onboardingDialog.open();
        }
    }

    // ── EULA Consent Dialog (first launch) ──────────────────────
    Dialog {
        id: eulaDialog
        title: "License Agreement"
        modal: true
        anchors.centerIn: parent
        width: 520
        height: 480
        padding: 28
        closePolicy: Popup.NoAutoClose

        background: Rectangle {
            radius: 14
            color: "#ffffff"
            border.color: "#e5e5ea"
            border.width: 1

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.6
                shadowVerticalOffset: 12
            }
        }

        contentItem: Column {
            spacing: 16
            width: parent.width

            Text {
                text: "📄 License Agreement"
                color: "#1d1d1f"
                font.pixelSize: 18
                font.weight: Font.Bold
            }

            Rectangle {
                width: parent.width; height: 1
                color: "#e5e5ea"
            }

            Flickable {
                width: parent.width
                height: 260
                clip: true
                contentHeight: eulaSummary.height

                Text {
                    id: eulaSummary
                    width: parent.width
                    text: "Kivo Music is licensed, not sold. This End User License Agreement (\"EULA\") is a legal agreement between you and the Kivo development team.\n\nBy clicking \"Accept\", you agree to:\n\n• Use this software for personal, non-commercial purposes\n• Not reverse engineer, decompile, or disassemble the software\n• Not distribute or transfer the software to third parties\n\nThis software is provided \"AS IS\" without warranty of any kind.\n\nFull EULA is available in Settings → License Agreement."
                    color: "#494949"
                    font.pixelSize: 13
                    lineHeight: 1.5
                    wrapMode: Text.WordWrap
                }
            }

            Text {
                text: "By continuing, you also acknowledge our Privacy Policy (available in Settings)."
                color: "#86868b"
                font.pixelSize: 11
                wrapMode: Text.WordWrap
                width: parent.width
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 12

                Button {
                    text: "Decline"
                    flat: true
                    onClicked: Qt.quit()

                    contentItem: Text {
                        text: "Decline"
                        color: "#86868b"
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        radius: 8; color: "#00000000"
                    }
                }

                Button {
                    text: "Accept"
                    onClicked: {
                        appSettings.eulaAccepted = true;
                        eulaDialog.close();
                        onboardingDialog.open();
                    }

                    contentItem: Text {
                        text: "Accept"
                        color: "#ffffff"
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        radius: 8
                        color: "#0071e3"
                    }
                }
            }
        }
    }

    // ── Onboarding Dialog ──────────────────────────────────────
    Dialog {
        id: onboardingDialog
        title: ""
        modal: true
        anchors.centerIn: parent
        width: 480
        height: 400
        padding: 32
        closePolicy: Popup.NoAutoClose

        property int step: 0

        background: Rectangle {
            radius: 14; color: "#ffffff"
            border.color: "#e5e5ea"; border.width: 1
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true; shadowColor: "#20000000"
                shadowBlur: 0.6; shadowVerticalOffset: 12
            }
        }

        contentItem: Column {
            spacing: 20
            width: parent.width

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: onboardingDialog.step === 0 ? "🎵" :
                      onboardingDialog.step === 1 ? "📂" : "⌨️"
                font.pixelSize: 56
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: onboardingDialog.step === 0 ? "Welcome to Kivo Music" :
                      onboardingDialog.step === 1 ? "Your Music Library" : "Power User Ready"
                color: "#1d1d1f"
                font.pixelSize: 20
                font.weight: Font.Bold
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: onboardingDialog.step === 0 ?
                      "A beautiful, bit-perfect music player\nfor your local collection." :
                      onboardingDialog.step === 1 ?
                      "Drag & drop audio files, or let Kivo scan\nyour music folders automatically." :
                      "Use keyboard shortcuts to control playback.\nSpace = Play/Pause, ←→ = Seek, Ctrl+M = Mini Player"
                color: "#86868b"
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                lineHeight: 1.5
            }

            // Step indicators
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 6

                Rectangle {
                    width: 8; height: 8; radius: 4
                    color: onboardingDialog.step >= 0 ? "#0071e3" : "#d0d0d5"
                }
                Rectangle {
                    width: 8; height: 8; radius: 4
                    color: onboardingDialog.step >= 1 ? "#0071e3" : "#d0d0d5"
                }
                Rectangle {
                    width: 8; height: 8; radius: 4
                    color: onboardingDialog.step >= 2 ? "#0071e3" : "#d0d0d5"
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 12

                Button {
                    text: "Skip"
                    flat: true
                    visible: onboardingDialog.step < 2
                    onClicked: {
                        appSettings.onboardingDone = true;
                        onboardingDialog.close();
                    }
                    contentItem: Text {
                        text: "Skip"
                        color: "#86868b"; font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle { radius: 8; color: "#00000000" }
                }

                Button {
                    text: onboardingDialog.step < 2 ? "Next" : "Get Started"
                    onClicked: {
                        if (onboardingDialog.step < 2) {
                            onboardingDialog.step++;
                        } else {
                            appSettings.onboardingDone = true;
                            onboardingDialog.close();
                        }
                    }
                    contentItem: Text {
                        text: onboardingDialog.step < 2 ? "Next" : "Get Started"
                        color: "#ffffff"; font.pixelSize: 14; font.weight: Font.DemiBold
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle { radius: 8; color: "#0071e3" }
                }
            }
        }
    }

    // ── Error Dialog (polished) ────────────────────────────────
    Dialog {
        id: errorDialog
        title: ""
        modal: true
        anchors.centerIn: parent
        padding: 24

        property string errorMessage: ""

        background: Rectangle {
            radius: 14
            color: "#ffffff"
            border.color: "#e5e5ea"
            border.width: 1

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.6
                shadowVerticalOffset: 12
            }
        }

        contentItem: Column {
            spacing: 16
            width: 380

            Row {
                spacing: 10
                Text {
                    text: "⚠️"
                    font.pixelSize: 24
                }
                Text {
                    text: "Audio Error"
                    color: "#cc0000"
                    font.pixelSize: 18
                    font.weight: Font.Bold
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: "#e5e5ea"
            }

            Text {
                id: errorText
                text: errorDialog.errorMessage
                wrapMode: Text.WordWrap
                width: parent.width
                color: "#494949"
                font.pixelSize: 13
                lineHeight: 1.4
            }
        }
    }

    // ── Audio Controller Connections ───────────────────────────
    Connections {
        target: audioController

        function onErrorOccurred(message) {
            console.error("Audio error:", message);
            errorDialog.errorMessage = message;
            errorDialog.open();
        }

        function onEndOfStream() {
            console.log("Track finished - auto-advancing");
        }

        function onFileOpened() {
            console.log("File opened successfully");
        }

        function onQueueFinished() {
            console.log("Queue finished");
        }
    }

    // ── Main Shell ─────────────────────────────────────────────
    HomeWindow {
        id: shell
        anchors.fill: parent
        onMinimizeRequested: window.showMinimized()
        onMaximizeRequested: {
            if (window.visibility === Window.Maximized)
                window.showNormal();
            else
                window.showMaximized();
        }
        onCloseRequested: window.close()
        onDragRequested: window.startSystemMove()
    }

    // ── Mini Player ────────────────────────────────────────────
    MiniPlayer {
        id: miniPlayer
        visible: false
    }

    // ── EQ Panel ───────────────────────────────────────────────
    EqPanel {
        id: eqPanel
    }

    // Ctrl+M: toggle mini player
    Shortcut {
        sequence: "Ctrl+M"
        onActivated: {
            if (miniPlayer.visible) {
                miniPlayer.hide();
                window.showNormal();
            } else {
                window.showMinimized();
                miniPlayer.show();
            }
        }
    }

    // Ctrl+E: toggle EQ
    Shortcut {
        sequence: "Ctrl+E"
        onActivated: {
            if (eqPanel.visible) eqPanel.hide();
            else eqPanel.show();
        }
    }
}
