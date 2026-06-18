import QtQuick
import QtQuick.Effects
import KivoMusic

Row {
    id: root
    property real volume: 0

    signal volumeRequested(real newVolume)

    spacing: 7

    // E5 无障碍 (Accessible 附加属性无 value/min/max,见 SeekBar 注释)
    Accessible.role: Accessible.Slider
    Accessible.name: qsTr("Volume") + " " + Math.round(root.volume * 100) + "%"


    Canvas {
        id: volIcon
        width: 18
        height: 18
        anchors.verticalCenter: parent.verticalCenter

        // 接入主题:之前硬编码深灰 #3a3e45,在暗色播放条上几乎不可见。
        property color iconColor: Theme.muted
        onIconColorChanged: requestPaint()

        onPaint: {
            const ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            ctx.strokeStyle = volIcon.iconColor;
            ctx.fillStyle = volIcon.iconColor;
            ctx.lineWidth = 1.35;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";
            ctx.beginPath();
            ctx.moveTo(width * 0.18, height * 0.42);
            ctx.lineTo(width * 0.34, height * 0.42);
            ctx.lineTo(width * 0.54, height * 0.26);
            ctx.lineTo(width * 0.54, height * 0.74);
            ctx.lineTo(width * 0.34, height * 0.58);
            ctx.lineTo(width * 0.18, height * 0.58);
            ctx.closePath();
            ctx.stroke();
            ctx.beginPath();
            ctx.arc(width * 0.58, height * 0.50, width * 0.16, -0.65, 0.65);
            ctx.stroke();
        }
    }

    Rectangle {
        id: volumeTrack
        width: 52
        height: volumeMouseArea.containsMouse || volumeMouseArea.pressed ? 4 : 3
        radius: height / 2
        color: Theme.transportTrack
        anchors.verticalCenter: parent.verticalCenter

        Behavior on height {
            NumberAnimation { duration: 120; easing.type: Easing.OutQuad }
        }

        Rectangle {
            id: volumeFill
            width: parent.width * Math.max(0, Math.min(1, root.volume))
            height: parent.height
            radius: parent.radius
            color: Theme.transportFill

            Behavior on width {
                enabled: !volumeMouseArea.pressed
                NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
            }
        }

        Rectangle {
            id: volumeHandle
            width: 10
            height: 10
            radius: 5
            color: Theme.npText
            border.color: Theme.transportFill
            border.width: 1
            x: Math.max(-width / 2, Math.min(volumeFill.width - width / 2, volumeTrack.width - width / 2))
            anchors.verticalCenter: parent.verticalCenter
            opacity: volumeMouseArea.containsMouse || volumeMouseArea.pressed ? 1.0 : 0.0
            scale: volumeMouseArea.pressed ? 1.2 : 1.0

            Behavior on opacity {
                NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
            }

            Behavior on scale {
                NumberAnimation { duration: 100; easing.type: Easing.OutBack }
            }

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#26000000"
                shadowBlur: 0.25
                shadowVerticalOffset: 2
            }
        }

        MouseArea {
            id: volumeMouseArea
            anchors.fill: parent
            anchors.margins: -6
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onPressed: function(mouse) {
                updateVolume(mouse.x);
            }

            onPositionChanged: function(mouse) {
                if (pressed) {
                    updateVolume(mouse.x);
                }
            }

            onClicked: function(mouse) {
                updateVolume(mouse.x);
            }

            function updateVolume(x) {
                // D6 修复: MouseArea 通过 anchors.margins: -6 向外扩展了 6px,
                // 直接用 x / width 包含了 margin 偏移和更宽的除数,导致音量偏低。
                // 正确做法: 映射回 volumeTrack 坐标系再归一化。
                const mapped = volumeMouseArea.mapToItem(volumeTrack, x, 0);
                const newVolume = Math.max(0, Math.min(1, mapped.x / volumeTrack.width));
                root.volumeRequested(newVolume);
            }
        }
    }
}
