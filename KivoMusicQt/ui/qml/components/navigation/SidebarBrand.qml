import QtQuick
import KivoMusic

Item {
    id: root
    height: 38

    Rectangle {
        id: logoTile
        width: 30
        height: 30
        radius: 8
        color: Theme.ink
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

        Text {
            anchors.centerIn: parent
            text: "K"
            color: Theme.brandK
            font.pixelSize: 15
            font.weight: Font.Bold
        }
    }

    Column {
        anchors.left: logoTile.right
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 1

        Text {
            width: parent.width
            text: "Kivo Music"
            color: Theme.text
            font.pixelSize: 15
            font.weight: Font.DemiBold
            elide: Text.ElideRight
        }

        Text {
            width: parent.width
            text: qsTr("Local Hi-Fi Library")
            color: Theme.muted
            font.pixelSize: 11
            elide: Text.ElideRight
        }
    }
}
