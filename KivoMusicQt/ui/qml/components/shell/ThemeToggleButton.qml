// =============================================================================
// Kivo Music - ThemeToggleButton.qml
// Shell component: Dark/Light mode toggle button
// =============================================================================

import QtQuick
import QtQuick.Controls
import "../../tokens"

Button {
    id: root
    width: 36
    height: 36
    flat: true
    text: ""
    font.pixelSize: 16

    Theme { id: theme }

    contentItem: Text {
        anchors.centerIn: parent
        text: theme.isDark ? "☀️" : "🌙"
        font.pixelSize: 16
    }

    background: Rectangle {
        radius: 8
        color: root.hovered ? theme.panelHover : "transparent"

        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }

    onClicked: theme.toggle()

    ToolTip {
        text: theme.isDark ? "Switch to Light Mode" : "Switch to Dark Mode"
        delay: 600
    }
}
