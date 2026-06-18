// =============================================================================
// Kivo Music - categories/language/LanguagePage.qml
// 职责: 显示语言选择（EN / zh_CN）。选中后写入 QSettings；提示重启生效。
// =============================================================================

import QtQuick
import KivoMusic

Flickable {
    id: page
    anchors.fill: parent
    contentWidth: width
    contentHeight: col.implicitHeight + 2 * Theme.space6
    clip: true

    property string pendingCode: settingsController.value("language", "")

    Column {
        id: col
        x: Theme.space6
        y: Theme.space6
        width: page.width - 2 * Theme.space6
        spacing: Theme.space6

        Text {
            text: qsTr("Language")
            color: Theme.text
            font.pixelSize: 24
            font.weight: Font.Bold
            font.family: Theme.fontFamily
        }

        SettingsSection {
            title: qsTr("Display Language")

            Repeater {
                model: [
                    { code: "en",    label: "English",  flag: "🇬🇧" },
                    { code: "zh_CN", label: "简体中文", flag: "🇨🇳" }
                ]

                delegate: Rectangle {
                    required property var modelData
                    width: parent.width
                    height: 48
                    color: langMouse.containsMouse ? Theme.panelHover : "transparent"
                    radius: Theme.radiusSmall
                    Behavior on color { ColorAnimation { duration: Theme.animFast } }

                    Row {
                        anchors.fill: parent
                        anchors.leftMargin: Theme.space4
                        anchors.rightMargin: Theme.space4
                        spacing: Theme.space3

                        Text {
                            text: modelData.flag
                            font.pixelSize: 18
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            text: modelData.label
                            color: Theme.text
                            font.pixelSize: 14
                            font.family: Theme.fontFamily
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width - 80
                        }
                        Text {
                            text: "✓"
                            font.pixelSize: 16
                            color: Theme.ctaPrimary
                            visible: page.pendingCode === modelData.code
                                  || (page.pendingCode === "" && modelData.code === "en")
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    MouseArea {
                        id: langMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            settingsController.setValue("language", modelData.code)
                            page.pendingCode = modelData.code
                        }
                    }
                }
            }
        }

        // Restart notice
        Rectangle {
            width: parent.width
            height: restartNote.implicitHeight + Theme.space4
            radius: Theme.radiusMedium
            color: Theme.panelSoft
            visible: page.pendingCode !== ""

            Text {
                id: restartNote
                anchors.centerIn: parent
                width: parent.width - Theme.space5
                text: qsTr("Restart Kivo Music to apply the language change.")
                color: Theme.textSecondary
                font.pixelSize: 13
                font.family: Theme.fontFamily
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
