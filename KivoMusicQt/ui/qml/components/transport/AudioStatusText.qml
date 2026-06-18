import QtQuick
import KivoMusic

Text {
    id: root
    property string statusText: ""


    text: root.statusText
    color: Theme.muted
    font.pixelSize: 10
    font.weight: Font.Medium
    elide: Text.ElideRight
    horizontalAlignment: Text.AlignRight
}
