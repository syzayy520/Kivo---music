import QtQuick

Item {
    id: root
    signal dragRequested()

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onPressed: root.dragRequested()
    }
}
