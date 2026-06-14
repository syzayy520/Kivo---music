import QtQuick
import "."
import "../../tokens"

Column {
    id: root
    property real contentWidth: width
    property var contentModel
    spacing: 14

    Theme { id: theme }

    Row {
        width: root.contentWidth

        Text {
            text: "Recently Played"
            color: theme.text
            font.pixelSize: 18
            font.weight: Font.DemiBold
        }

        Item { width: parent.width - 160; height: 1 }

        Text {
            text: "See All  >"
            color: theme.muted
            font.pixelSize: 14
        }
    }

    RecentListenStrip {
        width: root.contentWidth
        contentModel: root.contentModel
    }
}
