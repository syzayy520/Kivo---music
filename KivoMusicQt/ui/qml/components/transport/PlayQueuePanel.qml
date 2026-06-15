// PlayQueuePanel.qml
// Display and manage playback queue

import QtQuick
import QtQuick.Controls
import KivoMusic

Item {
    id: root
    
    visible: false
    anchors.fill: parent
    z: 100
    
    // Background dimming
    Rectangle {
        anchors.fill: parent
        color: "#80000000"
        opacity: root.visible ? 1.0 : 0.0
        
        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: root.visible = false
        }
    }
    
    // Panel
    Rectangle {
        width: 400
        height: parent.height
        x: root.visible ? parent.width - width : parent.width
        color: "#1a1a1a"
        border.color: "#333333"
        border.width: 1
        
        Behavior on x {
            NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
        }
        
        Column {
            anchors.fill: parent
            spacing: 0
            
            // Header
            Rectangle {
                width: parent.width
                height: 60
                color: "#222222"
                
                Text {
                    anchors.centerIn: parent
                    text: "Play Queue"
                    color: "#ffffff"
                    font.pixelSize: 20
                    font.bold: true
                }
                
                Button {
                    anchors.right: parent.right
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    text: "\u2715"
                    onClicked: root.visible = false
                }
            }
            
            // Queue list
            ListView {
                id: queueList
                width: parent.width
                height: parent.height - 60 - 80
                model: audioController.playQueueModel
                
                delegate: ItemDelegate {
                    width: queueList.width
                    height: 60
                    
                    background: Rectangle {
                        color: trackIndex === audioController.playQueueModel.currentIndex ? "#333333" : "transparent"
                    }
                    
                    Row {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10
                        
                        Text {
                            text: (trackIndex + 1) + "."
                            color: "#888888"
                            font.pixelSize: 14
                            width: 30
                        }
                        
                        Column {
                            Text {
                                text: displayName
                                color: trackIndex === audioController.playQueueModel.currentIndex ? "#1db954" : "#ffffff"
                                font.pixelSize: 16
                                font.bold: trackIndex === audioController.playQueueModel.currentIndex
                            }
                            
                            Text {
                                text: filePath.split("/").pop()
                                color: "#888888"
                                font.pixelSize: 11
                                elide: Text.ElideMiddle
                                width: 300
                            }
                        }
                    }
                    
                    onClicked: {
                        audioController.playQueueModel.setCurrentIndex(trackIndex)
                        audioController.openFile(filePath)
                    }
                }
            }
            
            // Playback mode bar
            Rectangle {
                width: parent.width
                height: 80
                color: "#222222"
                
                Row {
                    anchors.centerIn: parent
                    spacing: 20
                    
                    Button {
                        text: audioController.playbackModeModel.modeString
                        onClicked: audioController.togglePlaybackMode()
                    }
                }
            }
        }
    }
    
    function show() {
        root.visible = true
    }
    
    function hide() {
        root.visible = false
    }
}
