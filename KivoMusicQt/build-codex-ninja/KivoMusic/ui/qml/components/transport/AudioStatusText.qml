import QtQuick
import "../../tokens"

Text {
    id: root
    property string statusText: ""

    Theme { id: theme }

    text: root.statusText
    color: "#7c828b"
    font.pixelSize: 10
    font.weight: Font.Medium
    elide: Text.ElideRight
    horizontalAlignment: Text.AlignRight
}
