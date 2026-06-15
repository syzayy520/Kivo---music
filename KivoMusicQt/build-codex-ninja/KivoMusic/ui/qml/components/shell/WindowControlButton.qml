import QtQuick
import "../../tokens"

Rectangle {
    id: root
    property string controlType: "minimize"
    signal triggered()

    width: 42
    height: 32
    radius: 6
    color: controlType === "close" && mouseArea.containsMouse ? "#e5484d" : mouseArea.containsMouse ? "#edf0f4" : "transparent"

    // 添加平滑过渡
    Behavior on color {
        ColorAnimation { duration: 100; easing.type: Easing.OutQuad }
    }

    scale: mouseArea.pressed ? 0.95 : 1.0
    Behavior on scale {
        NumberAnimation { duration: 80; easing.type: Easing.OutQuad }
    }

    Theme { id: theme }

    Canvas {
        id: iconCanvas
        anchors.centerIn: parent
        width: 14
        height: 14

        onPaint: {
            const ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            ctx.strokeStyle = root.controlType === "close" && mouseArea.containsMouse ? "#ffffff" : theme.text;
            ctx.lineWidth = 1.45;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";

            if (root.controlType === "minimize") {
                ctx.beginPath();
                ctx.moveTo(3, 10);
                ctx.lineTo(11, 10);
                ctx.stroke();
                return;
            }

            if (root.controlType === "maximize") {
                ctx.strokeRect(3.5, 3.5, 7, 7);
                return;
            }

            ctx.beginPath();
            ctx.moveTo(4, 4);
            ctx.lineTo(10, 10);
            ctx.moveTo(10, 4);
            ctx.lineTo(4, 10);
            ctx.stroke();
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.triggered()
        onContainsMouseChanged: iconCanvas.requestPaint()
    }
}
