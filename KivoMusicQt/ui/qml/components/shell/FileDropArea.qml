// =============================================================================
// Kivo Music Qt - FileDropArea.qml
// Drop area for accepting audio files via drag-and-drop
// =============================================================================

import QtQuick

DropArea {
    id: root

    property var audioController: null
    signal filesDropped(var fileUrls)

    anchors.fill: parent
    keys: ["text/uri-list"]

    onEntered: function(drag) {
        drag.acceptProposedAction()
        dropOverlay.visible = true
    }

    onExited: {
        dropOverlay.visible = false
    }

    onDropped: function(drop) {
        dropOverlay.visible = false

        var urls = []
        for (var i = 0; i < drop.urls.length; i++) {
            urls.push(drop.urls[i].toString())
        }

        if (urls.length > 0 && audioController) {
            audioController.openFiles(urls)
        }
    }

    // Visual feedback overlay
    Rectangle {
        id: dropOverlay
        anchors.fill: parent
        color: "#20000000"
        visible: false
        z: 9999

        Rectangle {
            anchors.centerIn: parent
            width: 320
            height: 200
            radius: 20
            color: "#e6ffffff"
            border.color: "#40000000"
            border.width: 2

            Column {
                anchors.centerIn: parent
                spacing: 16

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "🎵"
                    font.pixelSize: 48
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Drop audio files here"
                    color: "#333333"
                    font.pixelSize: 20
                    font.bold: true
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "FLAC / MP3 / WAV / AAC"
                    color: "#888888"
                    font.pixelSize: 14
                }
            }
        }
    }
}
