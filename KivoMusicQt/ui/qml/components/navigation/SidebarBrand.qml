import QtQuick
import "../../tokens"

Row {
    id: root
    height: 38
    spacing: 10

    Theme { id: theme }

    Rectangle {
        width: 30
        height: 30
        radius: 8
        color: theme.ink
        anchors.verticalCenter: parent.verticalCenter

        Text {
            anchors.centerIn: parent
            text: "K"
            color: "#f4d06f"
            font.pixelSize: 15
            font.weight: Font.Bold
        }
    }

    Column {
        width: parent.width - 42
        anchors.verticalCenter: parent.verticalCenter
        spacing: 1

        Text {
            width: parent.width
            text: "Kivo Music"
            color: theme.text
            font.pixelSize: 15
            font.weight: Font.DemiBold
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: "Local Hi-Fi Library"
            color: theme.muted
            font.pixelSize: 11
            elide: Text.ElideRight
        }
    }
}
