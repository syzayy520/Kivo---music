// =============================================================================
// Kivo Music - windows/startup/SplashOverlay.qml
// 职责: 启动动画遮罩，800ms 停留后淡出，完成后发射 completed 信号。
// =============================================================================

import QtQuick
import KivoMusic

Rectangle {
    id: root

    signal completed

    color: Theme.page
    z: 9999
    opacity: 1

    Column {
        anchors.centerIn: parent
        spacing: 16

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "🎵"
            font.pixelSize: 48
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Kivo Music"
            color: Theme.text
            font.pixelSize: 28
            font.weight: Font.Bold
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Your music, elevated."
            color: Theme.faint
            font.pixelSize: 14
        }
    }

    Component.onCompleted: splashTimer.start()

    Timer {
        id: splashTimer
        interval: 800
        onTriggered: splashFadeOut.start()
    }

    SequentialAnimation {
        id: splashFadeOut
        NumberAnimation {
            target: root
            property: "opacity"
            to: 0
            duration: 400
            easing.type: Easing.OutQuad
        }
        ScriptAction {
            script: {
                root.visible = false;
                root.completed();
            }
        }
    }
}
