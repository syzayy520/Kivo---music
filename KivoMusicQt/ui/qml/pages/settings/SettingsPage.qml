// =============================================================================
// Kivo Music - SettingsPage.qml
// 职责: 设置页面（偏好设置入口 + 隐私/许可/关于链接）
// =============================================================================

import QtQuick
import QtQuick.Controls
import "../../tokens"

Item {
    id: root
    signal showPrivacyPolicy()
    signal showEula()
    signal showAbout()

    Theme { id: theme }

    Flickable {
        anchors.fill: parent
        contentHeight: settingsContent.height + 40
        clip: true

        Column {
            id: settingsContent
            width: parent.width - 60
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 40
            spacing: 28

            Text {
                text: "Settings"
                color: theme.text
                font.pixelSize: 28
                font.weight: Font.Bold
            }

            // ── Appearance ───────────────────────────────
            Text {
                text: "Appearance"
                color: theme.muted
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.capitalization: Font.AllUppercase
            }

            Rectangle {
                width: parent.width
                height: 56
                radius: 12
                color: theme.panel
                border.color: "#e5e5ea"

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    spacing: 12

                    Text {
                        text: "🎨"
                        font.pixelSize: 20
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: "Dark Mode"
                        color: theme.text
                        font.pixelSize: 14
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item { width: 1; height: 1 }  // spacer
                    Switch {
                        id: darkModeSwitch
                        anchors.verticalCenter: parent.verticalCenter
                        checked: theme.isDark
                        onToggled: theme.toggle()
                    }
                }
            }

            // ── Audio ────────────────────────────────────
            Text {
                text: "Audio"
                color: theme.muted
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.capitalization: Font.AllUppercase
            }

            Rectangle {
                width: parent.width
                height: 56
                radius: 12
                color: theme.panel
                border.color: "#e5e5ea"

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    spacing: 12

                    Text {
                        text: "🔊"
                        font.pixelSize: 20
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: "Audio Output Device"
                        color: theme.text
                        font.pixelSize: 14
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item { width: 1; height: 1 }  // spacer
                    Text {
                        text: "Default"
                        color: theme.faint
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            // ── Legal ────────────────────────────────────
            Text {
                text: "Legal & Information"
                color: theme.muted
                font.pixelSize: 11
                font.weight: Font.DemiBold
                font.capitalization: Font.AllUppercase
            }

            // Privacy Policy
            Rectangle {
                width: parent.width; height: 48; radius: 12
                color: privacyMouse.containsMouse ? "#f0f0f2" : theme.panel
                border.color: "#e5e5ea"

                Behavior on color { ColorAnimation { duration: 150 } }

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 16; anchors.rightMargin: 16
                    spacing: 12

                    Text { text: "🔒"; font.pixelSize: 18; anchors.verticalCenter: parent.verticalCenter }
                    Text {
                        text: "Privacy Policy"
                        color: theme.text; font.pixelSize: 14
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item { width: 1; height: 1 }  // spacer
                    Text { text: "›"; color: theme.faint; font.pixelSize: 18; anchors.verticalCenter: parent.verticalCenter }
                }

                MouseArea {
                    id: privacyMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.showPrivacyPolicy()
                }
            }

            // EULA
            Rectangle {
                width: parent.width; height: 48; radius: 12
                color: eulaMouse.containsMouse ? "#f0f0f2" : theme.panel
                border.color: "#e5e5ea"

                Behavior on color { ColorAnimation { duration: 150 } }

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 16; anchors.rightMargin: 16
                    spacing: 12

                    Text { text: "📄"; font.pixelSize: 18; anchors.verticalCenter: parent.verticalCenter }
                    Text {
                        text: "License Agreement"
                        color: theme.text; font.pixelSize: 14
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item { width: 1; height: 1 }  // spacer
                    Text { text: "›"; color: theme.faint; font.pixelSize: 18; anchors.verticalCenter: parent.verticalCenter }
                }

                MouseArea {
                    id: eulaMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.showEula()
                }
            }

            // About
            Rectangle {
                width: parent.width; height: 48; radius: 12
                color: aboutMouse.containsMouse ? "#f0f0f2" : theme.panel
                border.color: "#e5e5ea"

                Behavior on color { ColorAnimation { duration: 150 } }

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 16; anchors.rightMargin: 16
                    spacing: 12

                    Text { text: "ℹ️"; font.pixelSize: 18; anchors.verticalCenter: parent.verticalCenter }
                    Text {
                        text: "About Kivo Music"
                        color: theme.text; font.pixelSize: 14
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item { width: 1; height: 1 }  // spacer
                    Text { text: "›"; color: theme.faint; font.pixelSize: 18; anchors.verticalCenter: parent.verticalCenter }
                }

                MouseArea {
                    id: aboutMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.showAbout()
                }
            }

            Item { width: 1; height: 40 }
        }
    }
}
