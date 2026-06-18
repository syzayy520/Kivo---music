import QtQuick
import KivoMusic

Rectangle {
    id: root
    property real fadeWidth: 86

    width: fadeWidth
    z: 20
    color: "transparent"


    // 起点用「page 的透明版」而非硬编码浅色 —— 否则暗色模式下渐变会插值出灰色 ghost。
    gradient: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: Qt.rgba(Theme.page.r, Theme.page.g, Theme.page.b, 0) }
        GradientStop { position: 1.0; color: Theme.page }
    }
}
