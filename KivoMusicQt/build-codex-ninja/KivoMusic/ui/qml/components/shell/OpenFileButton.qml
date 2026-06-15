import QtQuick
import QtQuick.Effects
import "../../tokens"

Rectangle {
    id: root
    signal triggered()

    width: 90
    height: 32
    radius: 6
    color: theme.accent
    scale: mouseArea.pressed ? 0.95 : (mouseArea.containsMouse ? 1.02 : 1.0)
    opacity: mouseArea.pressed ? 0.9 : 1.0

    Theme { id: theme }

    Behavior on scale {
        NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
    }

    Behavior on opacity {
        NumberAnimation { duration: 100 }
    }

    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#50006cfa"
        shadowBlur: mouseArea.containsMouse ? 0.5 : 0.3
        shadowVerticalOffset: mouseArea.containsMouse ? 4 : 2

        Behavior on shadowBlur {
            NumberAnimation { duration: 150 }
        }

        Behavior on shadowVerticalOffset {
            NumberAnimation { duration: 150 }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true
        onClicked: root.triggered()
    }

    Text {
        text: "Open File"
        color: "#ffffff"
        font.pixelSize: 13
        font.weight: Font.Medium
        anchors.centerIn: parent
    }
}
