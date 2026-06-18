// =============================================================================
// Kivo Music Qt - EqSlider.qml
// 职责: 单个EQ频段垂直滑块
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic

Item {
    id: root
    property string bandLabel: "1k"
    property double bandGain: 0.0
    property double minGain: -12.0
    property double maxGain: 12.0

    signal gainChanged(double newGain)

    width: 40
    height: 180


    // Gain value display
    Text {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: bandGain.toFixed(1) + "dB"
        color: Math.abs(bandGain) < 0.5 ? Theme.muted : (bandGain > 0 ? Theme.eqPositive : Theme.eqNegative)
        font.pixelSize: 10
        font.weight: Font.Medium
    }

    // Slider
    Slider {
        id: slider
        anchors.top: parent.top
        anchors.topMargin: 18
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 24
        anchors.horizontalCenter: parent.horizontalCenter
        orientation: Qt.Vertical
        from: root.maxGain
        to: root.minGain
        value: root.bandGain
        stepSize: 0.5

        Accessible.name: root.bandLabel + qsTr(" EQ band")
        Accessible.description: bandGain.toFixed(1) + "dB"

        onValueChanged: root.gainChanged(value)

        background: Rectangle {
            width: 4
            height: parent.height
            radius: 2
            color: Theme.eqTrack
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                width: 4
                height: Math.abs(slider.value / slider.from) * parent.height
                radius: 2
                color: slider.value > 0 ? Theme.eqPositive : Theme.eqNegative
                anchors.bottom: slider.value > 0 ? parent.verticalCenter : undefined
                anchors.top: slider.value > 0 ? undefined : parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        handle: Rectangle {
            width: 12
            height: 12
            radius: 6
            color: Theme.eqHandle
            border.color: Theme.eqHandleBorder
            border.width: 1
            x: slider.leftPadding + (slider.availableWidth - width) / 2
            y: slider.topPadding + slider.visualPosition * (slider.availableHeight - height)
        }
    }

    // Band label
    Text {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: root.bandLabel
        color: Theme.muted
        font.pixelSize: 10
    }
}
