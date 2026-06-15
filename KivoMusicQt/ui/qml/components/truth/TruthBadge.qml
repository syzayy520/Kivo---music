import QtQuick
import "../../tokens"

Rectangle {
    id: root
    property string label: ""
    property string tone: "neutral"

    width: labelText.implicitWidth + 16
    height: 22
    radius: 5
    color: tone === "gold" ? "#f2d46b"
        : tone === "ok" ? "#dcebe4"
        : tone === "warn" ? "#f3dfd6"
        : "#ebe7df"

    Theme { id: theme }

    Text {
        id: labelText
        anchors.centerIn: parent
        text: root.label
        color: root.tone === "gold" ? "#4f3c00"
            : root.tone === "ok" ? "#285d50"
            : root.tone === "warn" ? "#91392f"
            : theme.muted
        font.pixelSize: 11
        font.weight: Font.DemiBold
    }
}
