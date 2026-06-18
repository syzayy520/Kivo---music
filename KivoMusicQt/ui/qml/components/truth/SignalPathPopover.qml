// =============================================================================
// Kivo Music - SignalPathPopover.qml
// Lightweight popover wrapper around the honest SignalPathPanel.
// =============================================================================

import QtQuick
import QtQuick.Controls
import "."
import KivoMusic

Popup {
    id: root

    padding: 0
    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Item {}  // the panel paints its own background

    contentItem: SignalPathPanel {}

    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 120 }
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 100 }
    }
}
