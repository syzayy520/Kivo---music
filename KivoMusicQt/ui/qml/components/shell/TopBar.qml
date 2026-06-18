import QtQuick
import QtQuick.Dialogs
import KivoMusic

Item {
    id: root
    property string pageTitle: ""
    signal minimizeRequested()
    signal maximizeRequested()
    signal closeRequested()
    signal dragRequested()
    signal searchRequested()


    FileDialog {
        id: fileDialog
        title: qsTr("Open Audio File")
        nameFilters: [qsTr("Audio Files") + " (*.flac *.mp3 *.m4a *.wav *.aac *.ogg *.opus)", qsTr("All Files") + " (*)"]
        fileMode: FileDialog.OpenFile

        onAccepted: {
            audioController.openFile(fileDialog.selectedFile.toString())
        }
    }

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

        // 打开文件(本地播放器的核心导入入口)。移除了原先无功能的 < / > 假导航箭头。
        OpenFileButton {
            anchors.verticalCenter: parent.verticalCenter
            onTriggered: fileDialog.open()
        }

        Text {
            text: root.pageTitle
            color: Theme.text
            font.pixelSize: 24
            font.weight: Font.DemiBold
            anchors.verticalCenter: parent.verticalCenter
            leftPadding: 6
        }
    }

    Rectangle {
        id: searchBox
        width: 232
        height: 32
        radius: 16
        // 弱化:更柔的底 + 更淡的发丝边,不与内容争视觉焦点。
        color: searchMa.containsMouse ? Theme.panelHover : Theme.panelSoft
        border.color: Theme.lineSubtle
        anchors.right: windowControls.left
        anchors.rightMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        Behavior on color { ColorAnimation { duration: Theme.animFast } }

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
                    ctx.strokeStyle = Theme.faint;
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
                text: qsTr("Search")
                color: Theme.faint
                font.pixelSize: 13
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            id: searchMa
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.searchRequested()
        }
    }

    Row {
        id: windowControls
        anchors.right: parent.right
        anchors.rightMargin: 14
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2

        // 主题切换已收进「设置 › 外观」,不再孤零零占据右上角(见设计批评)。

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
