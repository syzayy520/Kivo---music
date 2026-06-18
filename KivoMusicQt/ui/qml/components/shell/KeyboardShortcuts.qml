// =============================================================================
// Kivo Music Qt - KeyboardShortcuts.qml
// Global keyboard shortcuts for playback control
// =============================================================================

import QtQuick
import QtQuick.Controls

Item {
    id: root

    property var window: null
    // audioController is a global QML context property (set in main.cpp) — used
    // directly below. A local same-named property would self-reference (loop).

    // Capture all key events
    focus: true
    Keys.enabled: true

    Keys.onPressed: function(event) {
        if (!audioController) {
            return;
        }

        // Space: Play/Pause
        if (event.key === Qt.Key_Space) {
            event.accepted = true;
            audioController.togglePlayPause();
            return;
        }

        // Left arrow: Skip backward 5s
        if (event.key === Qt.Key_Left) {
            event.accepted = true;
            audioController.skipBackward(5);
            return;
        }

        // Right arrow: Skip forward 5s
        if (event.key === Qt.Key_Right) {
            event.accepted = true;
            audioController.skipForward(5);
            return;
        }

        // Up arrow: Volume up 5%
        if (event.key === Qt.Key_Up) {
            event.accepted = true;
            var newVol = Math.min(audioController.volume + 0.05, 1.0);
            audioController.setVolume(newVol);
            return;
        }

        // Down arrow: Volume down 5%
        if (event.key === Qt.Key_Down) {
            event.accepted = true;
            var newVol = Math.max(audioController.volume - 0.05, 0.0);
            audioController.setVolume(newVol);
            return;
        }

        // J: Previous track
        if (event.key === Qt.Key_J && event.modifiers === Qt.ControlModifier) {
            event.accepted = true;
            audioController.playPrevious();
            return;
        }

        // K: Play/Pause (Vim-style)
        if (event.key === Qt.Key_K && event.modifiers === Qt.ControlModifier) {
            event.accepted = true;
            audioController.togglePlayPause();
            return;
        }

        // L: Next track
        if (event.key === Qt.Key_L && event.modifiers === Qt.ControlModifier) {
            event.accepted = true;
            audioController.playNext();
            return;
        }

        // Media keys (if available)
        if (event.key === Qt.Key_MediaPlay || event.key === Qt.Key_MediaPause) {
            event.accepted = true;
            audioController.togglePlayPause();
            return;
        }

        if (event.key === Qt.Key_MediaNext) {
            event.accepted = true;
            audioController.playNext();
            return;
        }

        if (event.key === Qt.Key_MediaPrevious) {
            event.accepted = true;
            audioController.playPrevious();
            return;
        }

        if (event.key === Qt.Key_MediaStop) {
            event.accepted = true;
            audioController.stop();
            return;
        }

        // Ctrl+Q: Quit
        if (event.key === Qt.Key_Q && event.modifiers === Qt.ControlModifier) {
            event.accepted = true;
            if (window) {
                window.close();
            }
            return;
        }

        // Escape: Close error dialog / exit fullscreen
        if (event.key === Qt.Key_Escape) {
            event.accepted = true;
            if (window && window.visibility === Window.FullScreen) {
                window.showNormal();
            }
            return;
        }
    }
}
