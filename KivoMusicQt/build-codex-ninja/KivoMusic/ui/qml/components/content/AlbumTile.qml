import QtQuick
import QtQuick.Effects
import "../artwork"
import "../../tokens"

Item {
    id: root
    property string title: ""
    property string subtitle: ""
    property string note: ""
    property int artVariant: 0
    property url coverUrl: ""

    height: width + 58

    Theme { id: theme }

    // Hover 检测
    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onClicked: {
            // TODO: 打开专辑详情
        }
    }

    Column {
        anchors.fill: parent
        spacing: 7

        // 封面容器，添加 hover 效果
        Item {
            width: parent.width
            height: parent.width

            // Hover 抬升和阴影
            scale: hoverArea.containsMouse ? 1.04 : 1.0
            Behavior on scale {
                NumberAnimation { duration: 250; easing.type: Easing.OutCubic }
            }

            AlbumArtwork {
                id: artwork
                anchors.fill: parent
                radiusValue: 7
                variant: root.artVariant
                sourceUrl: root.coverUrl

                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: hoverArea.containsMouse ? "#35000000" : "#18000000"
                    shadowBlur: hoverArea.containsMouse ? 0.7 : 0.35
                    shadowVerticalOffset: hoverArea.containsMouse ? 12 : 4

                    Behavior on shadowColor {
                        ColorAnimation { duration: 250 }
                    }

                    Behavior on shadowBlur {
                        NumberAnimation { duration: 250 }
                    }

                    Behavior on shadowVerticalOffset {
                        NumberAnimation { duration: 250 }
                    }
                }
            }

            // Hover 播放按钮覆层
            Rectangle {
                anchors.centerIn: parent
                width: 48
                height: 48
                radius: 24
                color: "#ffffff"
                opacity: hoverArea.containsMouse ? 0.95 : 0.0

                Behavior on opacity {
                    NumberAnimation { duration: 200; easing.type: Easing.OutQuad }
                }

                scale: hoverArea.containsMouse ? 1.0 : 0.8
                Behavior on scale {
                    NumberAnimation { duration: 200; easing.type: Easing.OutBack }
                }

                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: "#40000000"
                    shadowBlur: 0.4
                    shadowVerticalOffset: 4
                }

                // 播放图标
                Canvas {
                    anchors.centerIn: parent
                    width: 20
                    height: 20

                    onPaint: {
                        const ctx = getContext("2d");
                        ctx.clearRect(0, 0, width, height);
                        ctx.fillStyle = theme.accent;
                        ctx.beginPath();
                        ctx.moveTo(width * 0.3, height * 0.2);
                        ctx.lineTo(width * 0.3, height * 0.8);
                        ctx.lineTo(width * 0.8, height * 0.5);
                        ctx.closePath();
                        ctx.fill();
                    }
                }
            }
        }

        Text {
            width: parent.width
            text: root.title
            color: theme.text
            font.pixelSize: 14
            font.weight: Font.DemiBold
            elide: Text.ElideRight
        }
        Text {
            width: parent.width
            text: root.subtitle
            color: theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }
        Text {
            width: parent.width
            text: root.note
            color: theme.muted
            font.pixelSize: 12
            elide: Text.ElideRight
        }
    }
}
