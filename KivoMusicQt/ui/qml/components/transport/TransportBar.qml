// =============================================================================
// Kivo Music - TransportBar.qml
// Full-width, bottom-flush player bar (Apple Music desktop idiom). Anchored —
// NOT a floating capsule — so it reads as part of the system, not a dialog.
// Frosted-glass fill + a single top hairline; track info left, controls center,
// right cluster right. Spectrum/lyrics float just above the bar during playback.
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import KivoMusic
import "../visualizer"

Item {
    id: root

    // Emitted when the user taps the track info — opens the immersive now-playing.
    signal expandRequested()
    // Emitted by the queue button — HomeWindow hosts the queue drawer at top level
    // (a thin transport bar can't host a full-height overlay).
    signal queueRequested()

    // ── Spectrum visualizer, floating just above the bar ─────────────────────
    // E9: 24-band spring-physics spectrum (aesthetic; real FFT pending).
    SpectrumView {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: bar.top
        anchors.bottomMargin: 8
        playing: audioController.playing
        visible: audioController.playing
        opacity: audioController.playing ? 1 : 0
        Behavior on opacity { NumberAnimation { duration: 200 } }
    }

    // ── Lyrics line, floating just above the bar ─────────────────────────────
    LyricsDisplay {
        id: lyricsDisplay
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: bar.top
        anchors.bottomMargin: 6
        width: Math.min(parent.width - 360, 600)
        currentText: lyricsController.currentLine
        nextText: lyricsController.nextLine
    }

    // ── Full-width bar ───────────────────────────────────────────────────────
    Rectangle {
        id: bar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: Theme.transportHeight

        // Frosted-glass fill — theme-aware, translucent so the page reads through.
        color: Theme.transportGlass

        // Single top hairline instead of an all-around drop shadow — this is what
        // makes it feel anchored to the window rather than floating above it.
        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color: Theme.transportGlassBorder
        }
        // Whisper-soft lift so the bar separates from bright content above it.
        Rectangle {
            anchors.bottom: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 6
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#00000000" }
                GradientStop { position: 1.0; color: Theme.isDark ? "#1a000000" : "#0d000000" }
            }
        }

        // ── Left: Track Info ──────────────────────────────────
        TransportTrackInfo {
            id: trackInfo
            width: 260
            title: audioController.title
            subtitle: audioController.artist
            coverUrl: audioController.coverArtPath
            playing: audioController.playing
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
        }

        // Tap the track info to open the immersive now-playing view.
        MouseArea {
            anchors.fill: trackInfo
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.expandRequested()
            ToolTip.visible: containsMouse
            ToolTip.text: qsTr("Open Now Playing")
        }

        // ── Center: Playback Controls ──────────────────────
        PlaybackControlCluster {
            id: controls
            elapsedText: audioController.elapsedText
            durationText: audioController.durationText
            progress: audioController.progress
            playing: audioController.playing
            loading: audioController.loading
            playbackModeValue: audioController.playbackModeValue
            anchors.centerIn: parent

            onPlayPauseClicked: audioController.togglePlayPause()
            onPreviousClicked: audioController.playPrevious()
            onNextClicked: audioController.playNext()
            onSeekRequested: function(position) {
                audioController.seekTo(position)
            }
            onShuffleClicked: audioController.toggleShuffle()
            onRepeatClicked: audioController.cycleRepeat()
        }

        // ── Right: Controls ───────────────────────────────
        TransportRightCluster {
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            volume: audioController.volume
            truthSampleRate: audioController.truthSampleRate
            onQueueRequested: root.queueRequested()
            onLyricsToggled: {
                // TODO: 打开歌词文件选择对话框，或自动查找同名.lrc
            }
            onVolumeRequested: (newVolume) => {
                audioController.volume = newVolume
                if (typeof settingsController !== "undefined")
                    settingsController.setValue("audio/volume", newVolume)
            }
        }
    }

    // ── Lyrics sync ticker ────────────────────────────────────────────────────
    Timer {
        interval: 200
        running: audioController.playing && lyricsController.hasLyrics
        repeat: true
        onTriggered: {
            // audioController 暂时没有 elapsedMs property，先用 progress * duration 估算
            // TODO: 添加 audioController.elapsedMs Q_PROPERTY
            const durationText = audioController.durationText  // "mm:ss"
            const parts = durationText.split(':')
            if (parts.length === 2) {
                const totalMs = (parseInt(parts[0]) * 60 + parseInt(parts[1])) * 1000
                const elapsedMs = totalMs * audioController.progress
                lyricsController.updatePosition(Math.floor(elapsedMs))
            }
        }
    }
}
