import QtQuick
import "."
import KivoMusic

Column {
    id: root
    property real contentWidth: width
    property var contentModel
    spacing: 14


    Item {
        width: root.contentWidth
        height: 24

        Text {
            text: qsTr("Recently Played")
            color: Theme.text
            font.pixelSize: Theme.fontSectionTitle
            font.weight: Font.Bold
            font.letterSpacing: -0.2
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: qsTr("All ›")
            color: Theme.text
            opacity: Theme.subtitleOpacity
            font.pixelSize: Theme.fontSubtitle
            font.weight: Font.Medium
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    RecentListenStrip {
        width: root.contentWidth
        contentModel: root.contentModel
    }
}
