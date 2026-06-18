// =============================================================================
// Kivo Music - PlayingPulse.qml
// A non-spectral now-playing indicator. The bars pulse on a fixed cadence driven
// ONLY by `active` (is-playing) — they are NOT derived from audio data and make
// no claim of spectrum analysis. Replaces the former random-number "spectrum".
// =============================================================================

import QtQuick
import KivoMusic

Row {
    id: root
    property bool active: false

    spacing: 4

    Repeater {
        model: 4
        Rectangle {
            id: bar
            width: 3
            radius: 1.5
            height: root.active ? 16 : 4
            anchors.verticalCenter: parent.verticalCenter
            color: root.active ? Theme.accent : Theme.faint
            opacity: root.active ? 1.0 : 0.5

            Behavior on height { NumberAnimation { duration: 260; easing.type: Easing.InOutSine } }

            SequentialAnimation on scale {
                running: root.active
                loops: Animation.Infinite
                NumberAnimation { from: 0.55; to: 1.0; duration: 420 + index * 110; easing.type: Easing.InOutSine }
                NumberAnimation { from: 1.0; to: 0.55; duration: 420 + index * 110; easing.type: Easing.InOutSine }
            }
        }
    }
}
