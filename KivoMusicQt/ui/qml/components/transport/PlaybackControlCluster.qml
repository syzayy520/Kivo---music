import QtQuick
import QtQuick.Effects
import "../common"
import KivoMusic

Column {
    id: root
    property string elapsedText: ""
    property string durationText: ""
    property real progress: 0
    property bool playing: false
    property bool loading: false
    // 0=Sequential 1=Shuffle 2=RepeatOne 3=RepeatAll (from audioController.playbackModeValue)
    property int playbackModeValue: 0

    signal playPauseClicked()
    signal previousClicked()
    signal nextClicked()
    signal seekRequested(real position)
    signal shuffleClicked()
    signal repeatClicked()

    // Wider than the old floating-capsule cluster so the scrubber has real
    // presence on the full-width bar (Apple Music gives the center a big chunk).
    width: 460
    spacing: 6


    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 12

        TransportButton {
            kind: "shuffle"
            active: root.playbackModeValue === 1  // Shuffle
            onClicked: root.shuffleClicked()
        }

        TransportButton {
            kind: "previous"
            onClicked: root.previousClicked()
        }

        TransportButton {
            kind: root.playing ? "pause" : "play"
            primary: true
            loading: root.loading
            onClicked: root.playPauseClicked()
        }

        TransportButton {
            kind: "next"
            onClicked: root.nextClicked()
        }

        TransportButton {
            // repeat-one shows the "1" badge; both repeat states tint active
            kind: root.playbackModeValue === 2 ? "repeat-one" : "repeat"
            active: root.playbackModeValue === 2 || root.playbackModeValue === 3
            onClicked: root.repeatClicked()
        }
    }

    Row {
        width: parent.width
        height: 14
        spacing: 9

        Text {
            width: 30
            text: root.elapsedText
            color: Theme.faint
            font.pixelSize: 10
            horizontalAlignment: Text.AlignRight
            anchors.verticalCenter: parent.verticalCenter
        }

        SeekBar {
            width: parent.width - 78
            anchors.verticalCenter: parent.verticalCenter
            progress: root.progress
            // 默认 trackColor/fillColor 即主题令牌,handle 白色 —— 与原播放条观感一致。
            onSeekRequested: function(position) { root.seekRequested(position) }
        }

        Text {
            width: 30
            text: root.durationText
            color: Theme.faint
            font.pixelSize: 10
            horizontalAlignment: Text.AlignLeft
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
