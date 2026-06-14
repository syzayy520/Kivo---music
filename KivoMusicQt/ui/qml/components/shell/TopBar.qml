import QtQuick
import "../../tokens"

Item {
    id: root
    property string pageTitle: ""
    signal minimizeRequested()
    signal maximizeRequested()
    signal closeRequested()
    signal dragRequested()

    Theme { id: theme }

    WindowDragRegion {
        anchors.fill: parent
        z: -1
        onDragRequested: root.dragRequested()
    }

    Row {
        anchors.left: parent.left
        anchors.leftMargin: 36
        anchors.verticalCenter: parent.verticalCenter
        spacing: 14

        Text {
            text: "<"
            font.pixelSize: 22
            color: theme.text
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: ">"
            font.pixelSize: 22
            color: theme.faint
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: root.pageTitle
            color: theme.text
            font.pixelSize: 30
            font.weight: Font.Bold
            anchors.verticalCenter: parent.verticalCenter
            leftPadding: 10
        }
    }

    Rectangle {
        width: 268
        height: 34
        radius: 17
        color: "#ffffff"
        border.color: theme.line
        anchors.right: windowControls.left
        anchors.rightMargin: 20
        anchors.verticalCenter: parent.verticalCenter

        Row {
            anchors.left: parent.left
            anchors.leftMargin: 13
            anchors.verticalCenter: parent.verticalCenter
            spacing: 8

            Canvas {
                width: 14
                height: 14
                anchors.verticalCenter: parent.verticalCenter
                onPaint: {
                    const ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);
                    ctx.strokeStyle = theme.faint;
                    ctx.lineWidth = 1.5;
                    ctx.beginPath();
                    ctx.arc(width * 0.43, height * 0.42, width * 0.28, 0, Math.PI * 2);
                    ctx.stroke();
                    ctx.beginPath();
                    ctx.moveTo(width * 0.63, height * 0.63);
                    ctx.lineTo(width * 0.84, height * 0.84);
                    ctx.stroke();
                }
            }

            Text {
                text: "Find in Kivo"
                color: theme.faint
                font.pixelSize: 13
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    Row {
        id: windowControls
        anchors.right: parent.right
        anchors.rightMargin: 14
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2

        WindowControlButton {
            controlType: "minimize"
            onTriggered: root.minimizeRequested()
        }

        WindowControlButton {
            controlType: "maximize"
            onTriggered: root.maximizeRequested()
        }

        WindowControlButton {
            controlType: "close"
            onTriggered: root.closeRequested()
        }
    }
}
