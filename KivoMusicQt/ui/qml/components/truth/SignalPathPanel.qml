import QtQuick
import QtQuick.Layouts
import "."
import "../../tokens"

Rectangle {
    id: root
    height: 74
    radius: 8
    color: "#fbfaf8"
    border.color: "#e1dcd5"

    Theme { id: theme }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 18
        anchors.rightMargin: 18
        spacing: 18

        Column {
            Layout.preferredWidth: 210
            Layout.maximumWidth: 230
            Layout.alignment: Qt.AlignVCenter
            spacing: 4
            Text {
                text: "Output Truth"
                color: theme.text
                font.pixelSize: 15
                font.weight: Font.DemiBold
            }
            Text {
                text: "Kivo AudioCore evidence summary"
                color: theme.muted
                font.pixelSize: 12
                elide: Text.ElideRight
            }
        }

        Rectangle {
            Layout.preferredWidth: 1
            height: 42
            color: theme.line
        }

        Flow {
            Layout.fillWidth: true
            Layout.minimumWidth: 420
            Layout.alignment: Qt.AlignVCenter
            spacing: 8
            TruthBadge { label: "Exclusive"; tone: "ok" }
            TruthBadge { label: "No engine"; tone: "ok" }
            TruthBadge { label: "96 kHz" }
            TruthBadge { label: "24-bit" }
            TruthBadge { label: "No conversion"; tone: "ok" }
            TruthBadge { label: "Evidence local"; tone: "neutral" }
        }

        Column {
            Layout.preferredWidth: 170
            Layout.maximumWidth: 180
            Layout.alignment: Qt.AlignVCenter
            spacing: 4
            Text {
                text: "Device"
                color: theme.muted
                font.pixelSize: 12
                horizontalAlignment: Text.AlignRight
                width: parent.width
            }
            Text {
                text: "WASAPI Exclusive"
                color: theme.text
                font.pixelSize: 14
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignRight
                width: parent.width
                elide: Text.ElideRight
            }
        }
    }
}
