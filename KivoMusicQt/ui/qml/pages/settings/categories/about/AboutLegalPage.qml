// =============================================================================
// Kivo Music - AboutLegalPage.qml
// 职责: 关于与法律。内部 StackView 承载既有 AboutPage/EulaPage/PrivacyPolicy
// （它们不再是顶层 currentPage 路由）。
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic

Item {
    id: page
    anchors.fill: parent

    // id is `aboutStack` (not `stack`) so the sub-page's own `stack` property can
    // bind to it without a name-collision self-reference (binding loop).
    StackView {
        id: aboutStack
        anchors.fill: parent
        initialItem: landing
    }

    Component {
        id: landing
        Flickable {
            contentWidth: width
            contentHeight: col.implicitHeight + 2 * Theme.space6
            clip: true

            Column {
                id: col
                x: Theme.space6
                y: Theme.space6
                width: parent.width - 2 * Theme.space6
                spacing: Theme.space6

                Text {
                    text: qsTr("About & Legal")
                    color: Theme.text
                    font.pixelSize: 24
                    font.weight: Font.Bold
                    font.family: Theme.fontFamily
                }

                SettingsSection {
                    title: qsTr("Information")

                    SettingsNavRow {
                        icon: "ℹ️"
                        label: qsTr("About Kivo Music")
                        onClicked: aboutStack.push(aboutComp)
                    }
                    SettingsNavRow {
                        icon: "🔒"
                        label: qsTr("Privacy Policy")
                        onClicked: aboutStack.push(privacyComp)
                    }
                    SettingsNavRow {
                        icon: "📄"
                        label: qsTr("License Agreement")
                        onClicked: aboutStack.push(eulaComp)
                    }
                }
            }
        }
    }

    Component {
        id: aboutComp
        SettingsSubPage {
            stack: aboutStack
            title: qsTr("About Kivo Music")
            AboutPage { anchors.fill: parent }
        }
    }
    Component {
        id: privacyComp
        SettingsSubPage {
            stack: aboutStack
            title: qsTr("Privacy Policy")
            PrivacyPolicy { anchors.fill: parent }
        }
    }
    Component {
        id: eulaComp
        SettingsSubPage {
            stack: aboutStack
            title: qsTr("License Agreement")
            EulaPage { anchors.fill: parent }
        }
    }
}
