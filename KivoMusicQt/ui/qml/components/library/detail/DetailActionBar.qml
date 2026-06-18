// =============================================================================
// Kivo Music - DetailActionBar.qml
// 职责: 详情页的「播放 / 随机」动作条。单一职责,只发两个意图信号,无业务逻辑。
// 由 album / artist 详情页共用。
// =============================================================================

import QtQuick
import KivoMusic

Row {
    id: root
    signal playClicked()
    signal shuffleClicked()
    spacing: 12

    // ── Play (filled accent pill) ────────────────────────────────────────────
    Rectangle {
        width: 128; height: 40; radius: 20
        color: playMa.containsMouse ? Qt.darker(Theme.accent, 1.08) : Theme.accent
        Behavior on color { ColorAnimation { duration: Theme.animFast } }

        Row {
            anchors.centerIn: parent
            spacing: 8
            Canvas {
                width: 12; height: 12
                anchors.verticalCenter: parent.verticalCenter
                onPaint: {
                    const c = getContext("2d"); c.clearRect(0, 0, 12, 12);
                    c.fillStyle = Theme.accentInk;
                    c.beginPath(); c.moveTo(2, 1); c.lineTo(2, 11); c.lineTo(11, 6);
                    c.closePath(); c.fill();
                }
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Play"); color: Theme.accentInk
                font.pixelSize: 14; font.weight: Font.DemiBold; font.family: Theme.fontFamily
            }
        }
        MouseArea {
            id: playMa; anchors.fill: parent; hoverEnabled: true
            cursorShape: Qt.PointingHandCursor; onClicked: root.playClicked()
        }
    }

    // ── Shuffle (outline pill) ───────────────────────────────────────────────
    Rectangle {
        width: 128; height: 40; radius: 20
        color: shufMa.containsMouse ? Theme.controlHover : "transparent"
        border.color: Theme.line; border.width: 1
        Behavior on color { ColorAnimation { duration: Theme.animFast } }

        Row {
            anchors.centerIn: parent
            spacing: 8
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "⤨"; color: Theme.text; font.pixelSize: 15
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Shuffle"); color: Theme.text
                font.pixelSize: 14; font.weight: Font.DemiBold; font.family: Theme.fontFamily
            }
        }
        MouseArea {
            id: shufMa; anchors.fill: parent; hoverEnabled: true
            cursorShape: Qt.PointingHandCursor; onClicked: root.shuffleClicked()
        }
    }
}
