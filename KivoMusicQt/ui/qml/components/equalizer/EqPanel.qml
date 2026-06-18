// =============================================================================
// Kivo Music Qt - EqPanel.qml
// 职责: 10段EQ均衡器完整面板
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import KivoMusic

Item {
    id: root
    visible: false
    anchors.fill: parent
    z: 100

    property var bandGains: [0,0,0,0,0,0,0,0,0,0]
    property var bandLabels: ["32","64","125","250","500","1k","2k","4k","8k","16k"]
    property string activePreset: "Flat"

    // Forward every gain change to the audio engine (no-op until DLL EQ support ships).
    onBandGainsChanged: {
        if (typeof audioController !== "undefined")
            audioController.setEqBands(bandGains)
    }

    // EQ preset lookup table (10-band, Hz: 32 64 125 250 500 1k 2k 4k 8k 16k).
    function presetGains(name) {
        var presets = {
            "Flat":       [ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0],
            "Rock":       [ 4,  3,  1, -1, -1,  1,  3,  4,  4,  3],
            "Pop":        [-1,  0,  2,  3,  3,  2,  0, -1, -1, -1],
            "Jazz":       [ 3,  2,  1,  1, -1, -1,  0,  1,  2,  2],
            "Classical":  [ 3,  2,  0, -1, -1, -1,  0,  2,  3,  4],
            "Vocal":      [-2, -2,  0,  2,  3,  3,  2,  1,  0, -1],
            "Bass Boost": [ 7,  6,  4,  2,  0, -1, -1, -1, -1, -1],
            "Treble":     [-1, -1, -1,  0,  0,  1,  2,  3,  5,  6]
        }
        return presets[name] || presets["Flat"]
    }


    // Dim background
    Rectangle {
        anchors.fill: parent
        color: Theme.npScrimHeavy
        opacity: root.visible ? 1.0 : 0.0
        Behavior on opacity { NumberAnimation { duration: 200 } }

        MouseArea {
            anchors.fill: parent
            onClicked: root.visible = false
        }
    }

    // Panel
    Rectangle {
        width: 600
        height: 360
        anchors.centerIn: parent
        radius: 16
        color: Theme.panel
        border.color: Theme.line
        border.width: 1

        // Shadow
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowBlur: 0.4
            shadowColor: Theme.shadowModal
            shadowVerticalOffset: 8
        }

        Column {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            // Title bar
            Item {
                width: parent.width
                height: 28

                Text {
                    text: qsTr("Equalizer")
                    font.pixelSize: 20
                    font.weight: Font.Bold
                    color: Theme.text
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                }

                Button {
                    text: "\u2715"
                    onClicked: root.visible = false
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    flat: true
                }
            }

            // Preset buttons
            Row {
                spacing: 8
                Repeater {
                    model: ["Flat","Rock","Pop","Jazz","Classical","Vocal","Bass Boost","Treble"]
                    Button {
                        text: modelData
                        flat: true
                        font.pixelSize: 11
                        font.weight: root.activePreset === modelData ? Font.Bold : Font.Normal

                        background: Rectangle {
                            radius: 6
                            color: root.activePreset === modelData ? Theme.eqPositive : Theme.lineSubtle
                        }

                        contentItem: Text {
                            text: parent.text
                            color: root.activePreset === modelData ? Theme.npText : Theme.muted
                            font: parent.font
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            root.activePreset = modelData;
                            var gains = root.presetGains(modelData);
                            for (var i = 0; i < gains.length; i++) {
                                root.bandGains[i] = gains[i];
                            }
                            root.bandGainsChanged();
                        }
                    }
                }
            }

            // EQ sliders
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8

                Repeater {
                    model: 10

                    EqSlider {
                        bandLabel: root.bandLabels[index]
                        bandGain: root.bandGains[index]
                        onGainChanged: function(newGain) {
                            root.bandGains[index] = newGain;
                            root.activePreset = "Custom";
                            root.bandGainsChanged()
                        }
                    }
                }
            }

            // Reset button
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 12

                Button {
                    text: qsTr("Reset to Flat")
                    flat: true
                    font.pixelSize: 12
                    onClicked: {
                        root.activePreset = "Flat";
                        for (var i = 0; i < 10; i++) root.bandGains[i] = 0;
                        root.bandGainsChanged();
                        if (typeof audioController !== "undefined")
                            audioController.setEqBands(root.bandGains);
                    }
                }

                Button {
                    text: qsTr("Save as Custom")
                    flat: true
                    font.pixelSize: 12
                    onClicked: {
                        console.log("EQ saved:", JSON.stringify(root.bandGains));
                    }
                }
            }
        }
    }

    function show() { root.visible = true; }
    function hide() { root.visible = false; }
}
