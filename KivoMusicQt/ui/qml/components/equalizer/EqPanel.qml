// =============================================================================
// Kivo Music Qt - EqPanel.qml
// 职责: 10段EQ均衡器完整面板
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import "../../tokens"

Item {
    id: root
    visible: false
    anchors.fill: parent
    z: 100

    property var bandGains: [0,0,0,0,0,0,0,0,0,0]
    property var bandLabels: ["32","64","125","250","500","1k","2k","4k","8k","16k"]
    property string activePreset: "Flat"

    Theme { id: theme }

    // Dim background
    Rectangle {
        anchors.fill: parent
        color: "#80000000"
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
        color: theme.panel
        border.color: "#e0e0e0"
        border.width: 1

        // Shadow
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowBlur: 0.4
            shadowColor: "#20000000"
            shadowVerticalOffset: 8
        }

        Column {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            // Title bar
            Row {
                width: parent.width
                spacing: 12

                Text {
                    text: "Equalizer"
                    font.pixelSize: 20
                    font.weight: Font.Bold
                    color: theme.text
                    anchors.verticalCenter: parent.verticalCenter
                }

                Item { width: 1; height: 1 }  // spacer

                Button {
                    text: "\u2715"
                    onClicked: root.visible = false
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
                            color: root.activePreset === modelData ? "#0071e3" : "#f0f0f0"
                        }

                        contentItem: Text {
                            text: parent.text
                            color: root.activePreset === modelData ? "white" : theme.muted
                            font: parent.font
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            root.activePreset = modelData;
                            // Apply preset gains
                            var gains = eqPresets.gainsFor(modelData);
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
                        }
                    }
                }
            }

            // Reset button
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 12

                Button {
                    text: "Reset to Flat"
                    flat: true
                    font.pixelSize: 12
                    onClicked: {
                        root.activePreset = "Flat";
                        for (var i = 0; i < 10; i++) root.bandGains[i] = 0;
                        root.bandGainsChanged();
                    }
                }

                Button {
                    text: "Save as Custom"
                    flat: true
                    font.pixelSize: 12
                    onClicked: {
                        // Persist custom EQ settings
                        console.log("EQ saved:", JSON.stringify(root.bandGains));
                    }
                }
            }
        }
    }

    function show() { root.visible = true; }
    function hide() { root.visible = false; }
}
