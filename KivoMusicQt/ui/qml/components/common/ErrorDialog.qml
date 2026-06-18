// =============================================================================
// Kivo Music - components/common/ErrorDialog.qml
// 职责: 通用错误提示对话框，调用方设置 errorMessage 后 open()。
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import KivoMusic

Dialog {
    id: root

    property string errorMessage: ""

    modal: true
    anchors.centerIn: parent
    padding: 24

    background: Rectangle {
        radius: 14
        color: Theme.panel
        border.color: Theme.line
        border.width: 1
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Theme.shadowModal
            shadowBlur: 0.6
            shadowVerticalOffset: 12
        }
    }

    contentItem: Column {
        spacing: 16
        width: 380

        Row {
            spacing: 10
            Text { text: "⚠️"; font.pixelSize: 24 }
            Text {
                text: "Audio Error"
                color: Theme.accentHot
                font.pixelSize: 18
                font.weight: Font.Bold
            }
        }

        Rectangle { width: parent.width; height: 1; color: Theme.line }

        Text {
            text: root.errorMessage
            wrapMode: Text.WordWrap
            width: parent.width
            color: Theme.textSecondary
            font.pixelSize: 13
            lineHeight: 1.4
        }
    }
}
