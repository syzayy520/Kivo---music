// =============================================================================
// Kivo Music - SourcesPage.qml
// 职责: 音乐来源（本地 + NAS/SMB 网络文件夹 + 局域网自动发现）。
// 绑定 C++ sourceManager / discovery 上下文属性。
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic

Flickable {
    id: page
    anchors.fill: parent
    contentWidth: width
    contentHeight: col.implicitHeight + 2 * Theme.space6
    clip: true

    Column {
        id: col
        x: Theme.space6
        y: Theme.space6
        width: page.width - 2 * Theme.space6
        spacing: Theme.space6

        Text {
            text: qsTr("Sources")
            color: Theme.text
            font.pixelSize: 24
            font.weight: Font.Bold
            font.family: Theme.fontFamily
        }

        // ── Configured folders ──────────────────────────────────────────
        SettingsSection {
            title: qsTr("Your Music Folders")

            Repeater {
                model: typeof sourceManager !== "undefined" ? sourceManager.roots : []

                delegate: SettingsEntityListRow {
                    required property var modelData
                    required property int index

                    width: col.width
                    icon: modelData.network ? "🗄️" : "💻"
                    title: modelData.label
                    subtitle: modelData.path
                    showToggle: modelData.network
                    toggleChecked: modelData.enabled
                    showRemove: modelData.removable
                    onToggled: sourceManager.setRootEnabled(index, value)
                    onRemoveClicked: sourceManager.removeRoot(index)
                }
            }

            // Add a network folder.
            Rectangle {
                id: addRow
                width: col.width
                height: 56
                radius: Theme.radiusXL
                color: Theme.panel
                border.color: Theme.line

                function commitPath() {
                    var raw = pathField.text
                    if (raw && raw.trim().length > 0
                            && typeof sourceManager !== "undefined") {
                        sourceManager.addNetworkFolder(raw, "")
                        pathField.text = ""
                    }
                }

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 12
                    spacing: Theme.space3

                    TextField {
                        id: pathField
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width - addBtn.width - 22
                        placeholderText: qsTr("\\\\192.168.5.1\\Music  or  smb://host/share")
                        color: Theme.text
                        placeholderTextColor: Theme.faint
                        font.pixelSize: 13
                        font.family: Theme.fontFamily
                        background: Rectangle { color: "transparent" }
                        onAccepted: addRow.commitPath()
                    }

                    Rectangle {
                        id: addBtn
                        anchors.verticalCenter: parent.verticalCenter
                        width: 88
                        height: 36
                        radius: Theme.radiusLarge
                        color: addMouse.containsMouse
                            ? Qt.darker(Theme.accent, 1.1) : Theme.accent
                        Behavior on color { ColorAnimation { duration: Theme.animFast } }

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Add Source")
                            color: Theme.accentInk
                            font.pixelSize: 13
                            font.weight: Font.DemiBold
                            font.family: Theme.fontFamily
                        }
                        MouseArea {
                            id: addMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: addRow.commitPath()
                        }
                    }
                }
            }
        }

        // ── LAN auto-discovery ───────────────────────────────────────────
        SettingsSection {
            title: qsTr("Network Discovery")

            Rectangle {
                width: col.width
                height: 44
                radius: Theme.radiusXL
                color: scanMouse.containsMouse ? Theme.panelHover : Theme.panel
                border.color: Theme.line
                Behavior on color { ColorAnimation { duration: Theme.animFast } }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.right: scanAction.left
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    elide: Text.ElideRight
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                    font.family: Theme.fontFamily
                    color: Theme.text
                    text: {
                        if (typeof discovery === "undefined")
                            return "🔍  " + qsTr("Scan the local network for NAS shares")
                        if (discovery.scanning)
                            return "⏳  " + discovery.statusText
                        return "🔍  " + (discovery.statusText.length > 0
                            ? discovery.statusText
                            : qsTr("Scan the local network for NAS shares"))
                    }
                }

                Text {
                    id: scanAction
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    text: (typeof discovery !== "undefined" && discovery.scanning)
                        ? qsTr("Cancel") : qsTr("Scan")
                    color: Theme.accent
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                    font.family: Theme.fontFamily
                }

                MouseArea {
                    id: scanMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (typeof discovery === "undefined") return
                        if (discovery.scanning) discovery.cancelScan()
                        else discovery.startScan()
                    }
                }
            }

            // Discovered shares — one-click add (persists like a manual entry).
            Repeater {
                model: typeof discovery !== "undefined" ? discovery.results : []

                delegate: Rectangle {
                    id: discRow
                    required property var modelData
                    required property int index

                    width: col.width
                    height: 52
                    radius: Theme.radiusXL
                    color: Theme.panelSoft
                    border.color: Theme.line

                    Text {
                        id: discIcon
                        text: "🗄️"
                        font.pixelSize: 18
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        id: addDiscBtn
                        width: 64
                        height: 30
                        radius: Theme.radiusMedium
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        color: discRow.modelData.added
                            ? "transparent"
                            : (addDiscMouse.containsMouse
                                ? Qt.darker(Theme.accent, 1.1) : Theme.accent)
                        border.color: discRow.modelData.added ? Theme.line : "transparent"

                        Text {
                            anchors.centerIn: parent
                            text: discRow.modelData.added ? qsTr("Added") : qsTr("Add")
                            color: discRow.modelData.added ? Theme.faint : Theme.accentInk
                            font.pixelSize: 12
                            font.weight: Font.DemiBold
                            font.family: Theme.fontFamily
                        }
                        MouseArea {
                            id: addDiscMouse
                            anchors.fill: parent
                            enabled: !discRow.modelData.added
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: discovery.addDiscovered(discRow.index)
                        }
                    }

                    Column {
                        anchors.left: discIcon.right
                        anchors.leftMargin: 12
                        anchors.right: addDiscBtn.left
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 1

                        Text {
                            text: discRow.modelData.share
                            color: Theme.text
                            font.pixelSize: 13
                            font.family: Theme.fontFamily
                            elide: Text.ElideRight
                            width: parent.width
                        }
                        Text {
                            text: discRow.modelData.path
                            color: Theme.faint
                            font.pixelSize: 11
                            font.family: Theme.fontFamily
                            elide: Text.ElideMiddle
                            width: parent.width
                        }
                    }
                }
            }
        }
    }
}
