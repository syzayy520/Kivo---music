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
