// =============================================================================
// Kivo Music - PlayQueueDrawer.qml
// 职责: 「播放队列」抽屉的组合根 —— 暗幕 + 右侧滑入面板 + open 状态,组装 header/list/modebar。
// E2: 拖动重排 —— 拖拽手柄(右侧 ⠿)按住后可垂直拖移改变曲目顺序,实时调用 moveTrack()。
// =============================================================================

import QtQuick
import KivoMusic

Item {
    id: drawer
    property bool open: false
    // E2 — index of the item currently being dragged (-1 = no drag in progress)
    property int _draggingIndex: -1

    visible: opacity > 0.01
    opacity: open ? 1.0 : 0.0
    Behavior on opacity { NumberAnimation { duration: 200 } }

    focus: open
    onOpenChanged: if (open) forceActiveFocus()
    Keys.onEscapePressed: function(event) { drawer.open = false; event.accepted = true }

    Rectangle {
        anchors.fill: parent
        color: Theme.scrim
        MouseArea {
            anchors.fill: parent
            onClicked: drawer.open = false
        }
    }

    Rectangle {
        id: panel
        width: Math.min(400, parent.width * 0.42)
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        x: drawer.open ? parent.width - width : parent.width
        color: Theme.panel
        Behavior on x { NumberAnimation { duration: 280; easing.type: Easing.OutCubic } }

        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: Theme.line
        }

        MouseArea { anchors.fill: parent }

        PlayQueueHeader {
            id: header
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            count: audioController.playQueueModel ? audioController.playQueueModel.count : 0
            onCloseRequested: drawer.open = false
        }

        PlayQueueModeBar {
            id: modeBar
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            mode: audioController.playbackModeValue
            onToggleRequested: audioController.togglePlaybackMode()
        }

        ListView {
            id: list
            anchors.top: header.bottom
            anchors.bottom: modeBar.top
            anchors.left: parent.left
            anchors.right: parent.right
            clip: true
            model: audioController.playQueueModel
            boundsBehavior: Flickable.StopAtBounds
            reuseItems: true
            cacheBuffer: 480

            // Animate displaced rows sliding out of the way during drag.
            moveDisplaced: Transition {
                NumberAnimation { properties: "y"; duration: 150; easing.type: Easing.OutCubic }
            }

            delegate: Item {
                id: rowSlot
                width: list.width
                height: 58
                required property int trackIndex
                required property string displayName
                required property string filePath
                required property string artist

                // Dim this slot while being dragged so the user sees which item moves.
                opacity: drawer._draggingIndex === rowSlot.trackIndex ? 0.45 : 1.0
                Behavior on opacity { NumberAnimation { duration: 100 } }

                PlayQueueRow {
                    id: rowDelegate
                    // Shrink right margin to leave room for the drag handle.
                    width: parent.width - 36
                    rowIndex: rowSlot.trackIndex
                    title: rowSlot.displayName
                    subtitle: rowSlot.artist
                    current: audioController.playQueueModel
                        && rowSlot.trackIndex === audioController.playQueueModel.currentIndex
                    onActivated: {
                        audioController.playQueueModel.setCurrentIndex(rowSlot.trackIndex)
                        audioController.openFile(rowSlot.filePath)
                    }
                }

                // ── Drag handle ─────────────────────────────────
                Item {
                    id: handleArea
                    width: 36; height: parent.height
                    anchors.right: parent.right

                    Text {
                        anchors.centerIn: parent
                        text: "⠿"
                        font.pixelSize: 13
                        font.family: Theme.fontFamily
                        color: dragMa.containsMouse || dragMa.pressed
                               ? Theme.muted : Theme.faint
                        Behavior on color { ColorAnimation { duration: 100 } }
                    }

                    MouseArea {
                        id: dragMa
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.SizeVerCursor
                        preventStealing: true

                        onPressed: {
                            drawer._draggingIndex = rowSlot.trackIndex
                        }

                        onPositionChanged: (event) => {
                            if (!pressed || drawer._draggingIndex < 0) return
                            // Map to list's content coordinates.
                            const listPos = dragMa.mapToItem(list.contentItem, mouseX, mouseY)
                            const rowHeight = 58
                            const newIdx = Math.max(0, Math.min(
                                list.count - 1,
                                Math.floor(listPos.y / rowHeight)))
                            if (newIdx !== drawer._draggingIndex) {
                                list.model.moveTrack(drawer._draggingIndex, newIdx)
                                drawer._draggingIndex = newIdx
                            }
                        }

                        onReleased: {
                            drawer._draggingIndex = -1
                        }
                    }
                }
            }
        }

        Text {
            anchors.centerIn: list
            visible: list.count === 0
            text: qsTr("Queue is empty")
            color: Theme.muted
            font.pixelSize: 14
            font.family: Theme.fontFamily
        }
    }
}
