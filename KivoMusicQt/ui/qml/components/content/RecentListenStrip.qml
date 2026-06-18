import QtQuick
import "../artwork"
import KivoMusic

ListView {
    id: root
    property var contentModel

    height: 50
    spacing: 24
    model: root.contentModel
    orientation: ListView.Horizontal
    boundsBehavior: Flickable.StopAtBounds
    flickDeceleration: 2800
    maximumFlickVelocity: 2400
    snapMode: ListView.SnapToItem
    clip: true


    delegate: Item {
        width: Math.max(190, Math.min(238, (root.width - 72) / 4.35))
        height: 50

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                if (model.filePath && model.filePath.length > 0)
                    audioController.openFile(model.filePath)
            }
        }

        AlbumArtwork {
            id: art
            width: 44
            height: 44
            radiusValue: 5
            variant: model.artVariant
            sourceUrl: model.coverUrl
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }

        Column {
            anchors.left: art.right
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 2

            Text {
                width: parent.width
                text: model.title
                color: Theme.text
                font.pixelSize: 13
                font.weight: Font.DemiBold
                elide: Text.ElideRight
            }

            Text {
                width: parent.width
                text: model.subtitle
                color: Theme.muted
                font.pixelSize: 12
                elide: Text.ElideRight
            }

            Text {
                width: parent.width
                text: model.note
                color: Theme.faint
                font.pixelSize: 11
                elide: Text.ElideRight
            }
        }

        Rectangle {
            width: 1
            height: 34
            color: Theme.line
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            visible: index < root.count - 1
        }
    }

    ShelfEdgeFade {
        height: parent.height
        anchors.right: parent.right
    }
}
