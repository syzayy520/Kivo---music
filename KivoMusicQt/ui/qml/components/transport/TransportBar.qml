import QtQuick
import QtQuick.Effects
import "../../tokens"

Item {
    id: root

    Theme { id: theme }

    Rectangle {
        id: capsule
        width: Math.min(parent.width - 260, 820)
        height: 58
        radius: height / 2
        color: "#eaf9fafb"
        border.color: "#f1cfd4dc"
        border.width: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#24000000"
            shadowBlur: 0.42
            shadowVerticalOffset: 12
            shadowHorizontalOffset: 0
        }

        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: "#ffffff"
            opacity: 0.34
        }

        Rectangle {
            width: parent.width - 46
            height: 1
            radius: 1
            color: "#ffffff"
            opacity: 0.72
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 1
        }

        Rectangle {
            width: parent.width - 56
            height: 1
            radius: 1
            color: "#d9dde3"
            opacity: 0.24
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
        }

        TransportTrackInfo {
            id: trackInfo
            width: 228
            title: transportState.title
            subtitle: transportState.subtitle
            artVariant: transportState.artVariant
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.verticalCenter: parent.verticalCenter
        }

        PlaybackControlCluster {
            id: controls
            elapsedText: transportState.elapsedText
            durationText: transportState.durationText
            progress: transportState.progress
            playing: transportState.playing
            anchors.centerIn: parent
        }

        Row {
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            spacing: 14

            AudioStatusText {
                width: 104
                statusText: transportState.formatText
                anchors.verticalCenter: parent.verticalCenter
            }

            VolumeCluster {
                volume: transportState.volume
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
