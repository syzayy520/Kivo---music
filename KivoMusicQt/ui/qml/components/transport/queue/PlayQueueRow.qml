// =============================================================================
// Kivo Music - PlayQueueRow.qml
// 职责: 播放队列里的「单条曲目行」—— 序号/正在播放指示、标题、文件名副标。
// 单一职责:只画一行 + 通过 activated 信号回报点击。主题感知,无硬编码颜色。
// =============================================================================

import QtQuick
import KivoMusic

Rectangle {
    id: row
    // 不叫 index:那会遮蔽 ListView 委托注入的 attached index(QML footgun)。
    property int rowIndex: 0
    property string title: ""
    property string subtitle: ""
    property bool current: false
    signal activated()

    height: 58
    color: mouse.containsMouse ? Theme.panelHover : "transparent"
    Behavior on color { ColorAnimation { duration: 120 } }

    // 正在播放:金色左条(与侧边栏选中同一套语言)。
    Rectangle {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        width: 3
        height: row.current ? 26 : 0
        radius: 2
        color: Theme.accent
        Behavior on height { NumberAnimation { duration: 180; easing.type: Easing.OutBack } }
    }

    Item {
        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        height: 40

        Item {
            id: indexSlot
            width: 22
            height: 22
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter

            Text {
                anchors.centerIn: parent
                visible: !row.current
                text: (row.rowIndex + 1)
                color: Theme.faint
                font.pixelSize: 13
                font.family: Theme.fontFamily
            }
            Text {
                id: nowPlayingSymbol
                anchors.centerIn: parent
                visible: row.current
                text: "♪"
                font.pixelSize: 15
                color: Theme.accent
                // E12 呼吸脉冲: 正在播放的曲目符号以 2s 周期轻微闪烁, 传达「活跃」感。
                SequentialAnimation on opacity {
                    running: row.current && audioController.playing
                    loops: Animation.Infinite
                    NumberAnimation { to: 0.45; duration: 1000; easing.type: Easing.InOutSine }
                    NumberAnimation { to: 1.0;  duration: 1000; easing.type: Easing.InOutSine }
                }
                onVisibleChanged: if (!visible) opacity = 1.0
            }
        }

        Column {
            anchors.left: indexSlot.right
            anchors.leftMargin: 12
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 2

            Text {
                width: parent.width
                text: row.title
                color: row.current ? Theme.accentText : Theme.text
                font.pixelSize: 14
                font.weight: row.current ? Font.DemiBold : Font.Normal
                font.family: Theme.fontFamily
                elide: Text.ElideRight
            }
            Text {
                width: parent.width
                text: row.subtitle
                color: Theme.muted
                font.pixelSize: 11
                font.family: Theme.fontFamily
                elide: Text.ElideMiddle
            }
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        Accessible.role: Accessible.ListItem
        Accessible.name: row.title
        Accessible.description: row.subtitle
        Accessible.selected: row.current
        Accessible.onPressAction: row.activated()
        onClicked: row.activated()
    }
}
