import QtQuick
import "../../tokens"

Rectangle {
    id: root
    property real fadeWidth: 86

    width: fadeWidth
    z: 20
    color: "transparent"

    Theme { id: theme }

    gradient: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: "#00fbfbfc" }
        GradientStop { position: 1.0; color: theme.page }
    }
}
