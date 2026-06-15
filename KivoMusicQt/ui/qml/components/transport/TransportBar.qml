// =============================================================================
// Kivo Music - TransportBar.qml
// Apple Music-level transport bar with live lyrics preview and polished glass
// =============================================================================

import QtQuick
import QtQuick.Effects
import "../../tokens"
import "../visualizer"

Item {
    id: root

    Theme { id: theme }

    // ── Audio Visualizer ──────────────────────────────────────
    // Apple Music style: 32-bar spectrum above the capsule
    Row {
        id: visualizerRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: capsule.top
        anchors.bottomMargin: 12
        spacing: 2
        visible: audioController.playing

        Repeater {
            model: 32

            WaveformBar {
                id: waveformBar
                targetHeight: {
                    if (!audioController.playing) return 4;
                    // 模拟频谱：中间高，两边低，随机波动
                    var centerDist = Math.abs(index - 15.5) / 15.5;
                    var baseHeight = 4 + (1 - centerDist) * 28;
                    var randomFactor = 0.6 + Math.random() * 0.8;
                    return baseHeight * randomFactor;
                }
                active: audioController.playing

                // ── 周期性刷新高度 ──────────────────────
                Timer {
                    interval: 120 + index * 15
                    repeat: true
                    running: audioController.playing
                    onTriggered: {
                        var centerDist = Math.abs(index - 15.5) / 15.5;
                        var baseHeight = 4 + (1 - centerDist) * 28;
                        var randomFactor = 0.6 + Math.random() * 0.8;
                        waveformBar.targetHeight = baseHeight * randomFactor;
                    }
                }
            }
        }
    }

    // ── Lyrics display above capsule ──────────────────────────
    LyricsDisplay {
        id: lyricsDisplay
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: capsule.top
        anchors.bottomMargin: 8
        width: Math.min(parent.width - 200, 600)
        currentText: ""
        nextText: ""
    }

    // ── Backdrop blur layer ──────────────────────────────────
    Rectangle {
        anchors.fill: capsule
        anchors.margins: -40
        color: "transparent"

        layer.enabled: true
        layer.effect: MultiEffect {
            blurEnabled: true
            blur: 1.4
            blurMax: 120
            blurMultiplier: 1.0
        }
    }

    // ── Main Capsule ─────────────────────────────────────────
    Rectangle {
        id: capsule
        width: Math.min(parent.width - 280, 860)
        height: 64
        radius: 16

        // Apple Music-style frosted glass
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#f4fafafa" }
            GradientStop { position: 0.5; color: "#f2f8f8f8" }
            GradientStop { position: 1.0; color: "#f0f6f6f6" }
        }
        border.color: "#14ffffff"
        border.width: 1

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 28

        // ── Enhanced Shadow ───────────────────────────────
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#3a000000"
            shadowBlur: 0.9
            shadowVerticalOffset: 28
            shadowHorizontalOffset: 0
        }

        // ── Top highlight edge ────────────────────────────
        Rectangle {
            width: parent.width - 40
            height: 1
            radius: 1
            color: "#ffffff"
            opacity: 0.8
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 1
        }

        // ── Bottom subtle line ────────────────────────────
        Rectangle {
            width: parent.width - 50
            height: 1
            radius: 1
            color: "#d0d3d8"
            opacity: 0.18
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
        }

        // ── Left: Track Info ──────────────────────────────
        TransportTrackInfo {
            id: trackInfo
            width: 240
            title: audioController.title
            subtitle: audioController.artist
            coverUrl: audioController.coverArtPath
            playing: audioController.playing
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
        }

        // ── Center: Playback Controls ──────────────────────
        PlaybackControlCluster {
            id: controls
            elapsedText: audioController.elapsedText
            durationText: audioController.durationText
            progress: audioController.progress
            playing: audioController.playing
            anchors.centerIn: parent

            onPlayPauseClicked: audioController.togglePlayPause()
            onPreviousClicked: audioController.playPrevious()
            onNextClicked: audioController.playNext()
            onSeekRequested: function(position) {
                audioController.seekTo(position)
            }
        }

        // ── Right: Controls ───────────────────────────────
        Row {
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10

            // Queue toggle
            TransportButton {
                id: queueBtn
                width: 28; height: 28
                kind: "queue"
                onClicked: playQueuePanel.visible = !playQueuePanel.visible
            }

            // Lyrics toggle
            TransportButton {
                id: lyricsBtn
                width: 28; height: 28
                kind: "lyrics"
                opacity: 1.0
                onClicked: {
                    // Toggle lyrics display — will be wired when lyrics data is available
                    lyricsDisplay.currentText = lyricsDisplay.currentText ? "" : "Lyrics coming soon...";
                    lyricsDisplay.nextText = "";
                }
            }

            AudioStatusText {
                width: 100
                statusText: audioController.formatText
                anchors.verticalCenter: parent.verticalCenter
            }

            VolumeCluster {
                volume: audioController.volume
                anchors.verticalCenter: parent.verticalCenter

                onVolumeRequested: function(newVolume) {
                    audioController.volume = newVolume
                }
            }
        }
    }

    // ── Play Queue Panel (overlay) ───────────────────────────
    PlayQueuePanel {
        id: playQueuePanel
        anchors.fill: parent
    }
}
