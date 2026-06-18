// =============================================================================
// Kivo Music - App.qml
// 职责: 根窗口 — 主题初始化、启动流程编排、全局快捷键。
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtCore
import KivoMusic
import "windows/startup"
import "components/common"

ApplicationWindow {
    id: window
    property alias currentPage: shell.currentPage

    width: 1440; height: 900
    minimumWidth: 1020; minimumHeight: 680
    visible: true
    title: "Kivo Music"
    color: Theme.page
    flags: Qt.Window | Qt.FramelessWindowHint

    // ── Theme persistence ──────────────────────────────────────
    Component.onCompleted: {
        if (typeof settingsController !== "undefined") {
            Theme.isDark = settingsController.value("appearance/darkMode", false)
            audioController.setVolume(settingsController.value("audio/volume", 0.7))
        }
    }
    Connections {
        target: Theme
        function onIsDarkChanged() {
            if (typeof settingsController !== "undefined"
                    && settingsController.value("appearance/darkMode", false) !== Theme.isDark)
                settingsController.setValue("appearance/darkMode", Theme.isDark)
        }
    }

    // ── Lyrics auto-load: watch audioController.title → load same-name .lrc ─────
    Connections {
        target: audioController
        function onTitleChanged() {
            const queue = audioController.playQueueModel
            if (!queue || queue.currentIndex < 0) { lyricsController.clear(); return }
            const filePath = queue.getTrackAt(queue.currentIndex)
            if (!filePath) { lyricsController.clear(); return }
            lyricsController.loadLrcFile(filePath.replace(/\.[^.]+$/, ".lrc"))
        }
    }

    // ── Persisted settings ─────────────────────────────────────
    Settings {
        id: appSettings
        property bool eulaAccepted: false
        property bool onboardingDone: false
        property string appVersion: "1.0.0"
        category: "KivoMusic"
    }

    // ── Global Keyboard Shortcuts ──────────────────────────────
    KeyboardShortcuts { id: keyboardShortcuts; window: window }

    // ── Startup flow ───────────────────────────────────────────
    function checkFirstRun() {
        if (!appSettings.eulaAccepted)       eulaDialog.open();
        else if (!appSettings.onboardingDone) onboardingDialog.open();
    }

    SplashOverlay { anchors.fill: parent; onCompleted: checkFirstRun() }

    EulaDialog {
        id: eulaDialog
        onEulaAccepted: { appSettings.eulaAccepted = true; onboardingDialog.open() }
        onEulaDeclined: Qt.quit()
    }

    OnboardingDialog {
        id: onboardingDialog
        onOnboardingCompleted: { appSettings.onboardingDone = true; close() }
    }

    // ── Audio errors ───────────────────────────────────────────
    ErrorDialog { id: errorDialog }

    Connections {
        target: audioController
        function onErrorOccurred(message) {
            console.error("Audio error:", message);
            errorDialog.errorMessage = message;
            errorDialog.open();
        }
        function onEndOfStream()   { console.log("Track finished - auto-advancing") }
        function onFileOpened()    { console.log("File opened successfully") }
        function onQueueFinished() { console.log("Queue finished") }
    }

    // ── Main Shell ─────────────────────────────────────────────
    HomeWindow {
        id: shell
        anchors.fill: parent
        onMinimizeRequested:  window.showMinimized()
        onMaximizeRequested:  window.visibility === Window.Maximized ? window.showNormal() : window.showMaximized()
        onCloseRequested:     window.close()
        onDragRequested:      window.startSystemMove()
    }

    // ── Overlays ───────────────────────────────────────────────
    MiniPlayer { id: miniPlayer; visible: false }
    EqPanel    { id: eqPanel }

    Shortcut {
        sequence: "Ctrl+M"
        onActivated: {
            if (miniPlayer.visible) { miniPlayer.hide(); window.showNormal() }
            else { window.showMinimized(); miniPlayer.show() }
        }
    }
    Shortcut {
        sequence: "Ctrl+E"
        onActivated: eqPanel.visible ? eqPanel.hide() : eqPanel.show()
    }
}
