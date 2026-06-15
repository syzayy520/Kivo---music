import QtQuick
import QtQuick.Effects
import "../../tokens"

Item {
    id: root

    Theme { id: theme }

    // Backdrop blur layer (姣涚幓鐠冭儗鏅?
    Rectangle {
        anchors.fill: capsule
        anchors.margins: -20
        color: "transparent"

        layer.enabled: true
        layer.effect: MultiEffect {
            blurEnabled: true
            blur: 1.0
            blurMax: 64
            blurMultiplier: 1.0
        }
    }

    Rectangle {
        id: capsule
        width: Math.min(parent.width - 260, 820)
        height: 58
        radius: height / 2

        // 姣涚幓鐠冩晥鏋滐細鍗婇€忔槑鐧借壊 + 楂樻柉妯＄硦
        color: "#f5ffffff"
        border.color: "#18000000"
        border.width: 1

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#30000000"
            shadowBlur: 0.6
            shadowVerticalOffset: 16
            shadowHorizontalOffset: 0
        }

        // 椤堕儴楂樺厜
        Rectangle {
            width: parent.width - 46
            height: 1
            radius: 1
            color: "#ffffff"
            opacity: 0.85
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 1
        }

        // 搴曢儴缁嗙嚎
        Rectangle {
            width: parent.width - 56
            height: 1
            radius: 1
            color: "#d9dde3"
            opacity: 0.18
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
        }

        TransportTrackInfo {
            id: trackInfo
            width: 228
            title: audioController.title
            subtitle: audioController.artist
            artVariant: 0  // TODO: integrate with album art
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.verticalCenter: parent.verticalCenter
        }

        PlaybackControlCluster {
            id: controls
            elapsedText: audioController.elapsedText
            durationText: audioController.durationText
            progress: audioController.progress
            playing: audioController.playing
            anchors.centerIn: parent

            onPlayPauseClicked: audioController.togglePlayPause()
            onPreviousClicked: {
                // TODO: previous track
            }
            onNextClicked: {
                // TODO: next track
            }
            onSeekRequested: function(position) {
                audioController.seekTo(position)
            }
        }

        Row {
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            spacing: 14

            AudioStatusText {
                width: 104
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
}

