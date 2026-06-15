// =============================================================================
// Kivo Music - LyricsDisplay.qml
// Transport component: Apple Music-style inline lyrics display
// =============================================================================

import QtQuick
import "../../tokens"

Item {
    id: root
    property string currentText: ""
    property string nextText: ""
    property bool hasContent: currentText.length > 0

    height: hasContent ? 44 : 0
    clip: true

    Theme { id: theme }

    Behavior on height {
        NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
    }

    Column {
        anchors.centerIn: parent
        width: parent.width
        spacing: 2

        // Current line
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.currentText
            color: theme.text
            font.pixelSize: 14
            font.weight: Font.DemiBold
            maximumLineCount: 1
            elide: Text.ElideRight
            width: parent.width - 40
            horizontalAlignment: Text.AlignHCenter

            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
        }

        // Next line (dimmed)
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.nextText
            color: theme.muted
            font.pixelSize: 12
            font.weight: Font.Normal
            maximumLineCount: 1
            elide: Text.ElideRight
            width: parent.width - 40
            horizontalAlignment: Text.AlignHCenter
            opacity: root.nextText.length > 0 ? 0.6 : 0
        }
    }
}
