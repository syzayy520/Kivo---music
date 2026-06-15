import QtQuick
import QtQuick.Controls
import KivoMusic

ApplicationWindow {
    id: window
    property alias currentPage: shell.currentPage

    width: 1440
    height: 900
    minimumWidth: 1180
    minimumHeight: 760
    visible: true
    title: "Kivo Music"
    color: "#fbfbfc"
    flags: Qt.Window | Qt.FramelessWindowHint

    // Error dialog
    Dialog {
        id: errorDialog
        title: "Audio Error"
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Ok

        property string errorMessage: ""

        contentItem: Text {
            text: errorDialog.errorMessage
            wrapMode: Text.WordWrap
            width: 300
        }
    }

    // Audio controller connections
    Connections {
        target: audioController

        function onErrorOccurred(message) {
            console.error("Audio error:", message);
            errorDialog.errorMessage = message;
            errorDialog.open();
        }

        function onEndOfStream() {
            console.log("Playback finished");
            // TODO: Play next track in queue
        }

        function onFileOpened() {
            console.log("File opened successfully");
        }
    }

    HomeWindow {
        id: shell
        anchors.fill: parent
        onMinimizeRequested: window.showMinimized()
        onMaximizeRequested: {
            if (window.visibility === 4)
                window.showNormal();
            else
                window.showMaximized();
        }
        onCloseRequested: window.close()
        onDragRequested: window.startSystemMove()
    }
}
