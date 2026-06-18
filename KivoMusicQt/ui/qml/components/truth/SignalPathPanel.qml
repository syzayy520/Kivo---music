// =============================================================================
// Kivo Music - SignalPathPanel.qml
// The audio-truth MOAT, shown as a visual signal chain (Source → Decode → Engine
// → Output) — the thing Apple Music never reveals. Every value is evidence-gated
// (ABI 1.2.0): a stage is GOLD only when proven intact, AMBER when proven broken,
// neutral grey when unknown. No hardcoded claims; colour IS the verdict.
// =============================================================================

import QtQuick
import "."
import KivoMusic

Rectangle {
    id: root
    implicitWidth: 360
    implicitHeight: content.implicitHeight + 2 * Theme.space5
    radius: Theme.radiusLarge
    color: Theme.panel
    border.color: Theme.line
    border.width: 1

    readonly property bool complete: audioController.truthEvidenceComplete

    // ── evidence → display helpers (honest; "Unknown" wherever unproven) ──────
    function _sourceValue() {
        var parts = []
        var f = audioController.formatText
        if (f && f !== "Ready" && f !== "Playing" && f !== "Unknown") parts.push(f)
        if (/[0-9]/.test(audioController.truthSampleRate)) parts.push(audioController.truthSampleRate)
        if (/[0-9]/.test(audioController.truthBitDepth)) parts.push(audioController.truthBitDepth)
        return parts.length ? parts.join("  ·  ") : qsTr("Unknown")
    }
    function _decodeState() {
        if (!complete) return 0
        var c = audioController.truthConversionActive
        return c === "No" ? 1 : c === "Yes" ? -1 : 0
    }
    function _engineState() {
        if (!complete) return 0
        var p = audioController.truthProcessingActive
        var m = audioController.truthEngineInPath
        return (p === "No" && m === "No") ? 1 : (p === "Yes" || m === "Yes") ? -1 : 0
    }
    function _outputState() {
        if (!complete) return 0
        var a = audioController.truthActualMode
        return a === "Exclusive" ? 1 : a === "Shared" ? -1 : 0
    }

    Column {
        id: content
        anchors.fill: parent
        anchors.margins: Theme.space5
        spacing: Theme.space4

        // ── Header: title + live verdict badge ───────────────────────────────
        Item {
            width: parent.width
            height: 24
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Signal Path")
                color: Theme.text
                font.pixelSize: 15
                font.weight: Font.Bold
                font.letterSpacing: -0.2
                font.family: Theme.fontFamily
            }
            TruthBadge {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: audioController.truthVerdict
            }
        }

        Text {
            width: parent.width
            text: root.complete
                ? qsTr("Verified end-to-end from Kivo AudioCore evidence")
                : qsTr("Evidence incomplete — the path is not yet verified")
            color: Theme.muted
            font.pixelSize: 11
            font.family: Theme.fontFamily
            wrapMode: Text.WordWrap
        }

        // ── The chain ─────────────────────────────────────────────────────────
        Column {
            width: parent.width
            spacing: 0

            SignalPathStage {
                title: qsTr("Source")
                value: root._sourceValue()
                state: root.complete ? 1 : 0
            }
            SignalPathStage {
                title: qsTr("Decode")
                value: !root.complete ? qsTr("Unknown")
                    : audioController.truthConversionActive === "No" ? qsTr("Lossless · no resampling")
                    : audioController.truthConversionActive === "Yes" ? qsTr("Resampling the sample rate")
                    : qsTr("Unknown")
                state: root._decodeState()
            }
            SignalPathStage {
                title: qsTr("Engine")
                value: !root.complete ? qsTr("Unknown")
                    : (audioController.truthProcessingActive === "No"
                       && audioController.truthEngineInPath === "No") ? qsTr("No DSP · system mixer bypassed")
                    : audioController.truthProcessingActive === "Yes" ? qsTr("Audio processing is active")
                    : audioController.truthEngineInPath === "Yes" ? qsTr("System mixer is in the path")
                    : qsTr("Unknown")
                state: root._engineState()
            }
            SignalPathStage {
                title: qsTr("Output")
                value: !root.complete ? qsTr("Unknown")
                    : audioController.truthActualMode === "Exclusive" ? qsTr("WASAPI Exclusive · direct to device")
                    : audioController.truthActualMode === "Shared" ? qsTr("Shared mode · via system mixer")
                    : qsTr("Unknown")
                state: root._outputState()
                isLast: true
            }
        }

        // ── Why-not explanation (only for a complete, rejected verdict) ───────
        Rectangle {
            width: parent.width
            visible: audioController.truthRejectionReason !== ""
            height: visible ? reason.implicitHeight + 2 * Theme.space3 : 0
            radius: Theme.radiusMedium
            color: Theme.lineSubtle

            Text {
                id: reason
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: Theme.space3
                text: "⚠  " + audioController.truthRejectionReason
                color: Theme.muted
                font.pixelSize: 11
                font.family: Theme.fontFamily
                wrapMode: Text.WordWrap
            }
        }
    }
}
