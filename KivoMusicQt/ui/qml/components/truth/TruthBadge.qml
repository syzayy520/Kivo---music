// =============================================================================
// Kivo Music - TruthBadge.qml
// The bit-perfect verdict chip. The ONLY component allowed to use the verified
// (gold) tone, and only for the exact "Bit-Perfect" string — callers cannot pass
// a precomputed favourable tone.
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic

Rectangle {
    id: root
    property string text: "Unknown"

    // Tone is DERIVED from the text, never supplied — honesty chokepoint.
    readonly property string _tone: text === "Bit-Perfect" ? "gold"
        : text === "Not Bit-Perfect" ? "warn"
        : "neutral"


    // "warn" (Not Bit-Perfect) is caution, not error — warm amber, not system red,
    // matching the amber signal-path stages.
    readonly property color _warnTone: "#cf7a2e"

    implicitWidth: badgeText.implicitWidth + 20
    implicitHeight: 24
    radius: Theme.radiusSmall
    color: _tone === "gold" ? Qt.rgba(1.0, 0.624, 0.039, 0.16)
        : _tone === "warn" ? Qt.rgba(0.88, 0.57, 0.25, 0.15)
        : Theme.panelSoft
    border.width: 1
    border.color: _tone === "gold" ? Theme.accentGold
        : _tone === "warn" ? _warnTone
        : Theme.line

    Text {
        id: badgeText
        anchors.centerIn: parent
        text: root.text
        color: root._tone === "gold" ? Theme.accentGold
            : root._tone === "warn" ? root._warnTone
            : Theme.muted
        font.pixelSize: 12
        font.weight: Font.DemiBold
        font.family: Theme.fontFamily
    }

    Accessible.role: Accessible.StaticText
    Accessible.name: "Bit-perfect verdict: " + root.text
}
