// =============================================================================
// Kivo Music Qt - FileDropArea.qml
// Drop area for accepting audio files via drag-and-drop
// =============================================================================

import QtQuick
import KivoMusic

DropArea {
    id: root

    // audioController is a global QML context property (set in main.cpp); use it
    // directly. A local same-named property + `audioController: audioController`
    // binding self-references and triggers a binding loop.
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
        color: Theme.scrim
        visible: false
        z: 9999

        Rectangle {
            anchors.centerIn: parent
            width: 320
            height: 200
            radius: 20
            color: Theme.panel
            border.color: Theme.line
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
                    text: qsTr("Drop audio files here")
                    color: Theme.text
                    font.pixelSize: 20
                    font.weight: Font.Bold
                    font.family: Theme.fontFamily
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "FLAC · MP3 · WAV · AAC"
                    color: Theme.muted
                    font.pixelSize: 14
                    font.family: Theme.fontFamily
                }
            }
        }
    }
}
