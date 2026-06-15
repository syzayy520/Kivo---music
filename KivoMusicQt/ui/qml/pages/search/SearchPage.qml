import QtQuick
import QtQuick.Controls
import "../../components/search"
import "../../tokens"

Item {
    id: root

    Theme { id: theme }

    ScrollView {
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth

        Column {
            id: pageColumn
            property int contentMargin: 36
            readonly property real contentWidth: width - contentMargin * 2

            width: parent.width
            spacing: 22

            Rectangle {
                x: pageColumn.contentMargin
                width: Math.min(620, pageColumn.contentWidth)
                height: 42
                radius: 21
                color: "#ffffff"
                border.color: theme.line

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 18
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Search artists, albums, songs, and playlists"
                    color: theme.faint
                    font.pixelSize: 14
                }
            }

            Row {
                x: pageColumn.contentMargin
                width: pageColumn.contentWidth
                spacing: 32

                Column {
                    width: Math.min(640, parent.width * 0.58)
                    spacing: 8

                    Text {
                        text: "Recent Results"
                        color: theme.text
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                    }

                    Repeater {
                        model: searchModels.results

                        SearchResultRow {
                            width: parent.width
                            title: model.title
                            detail: model.detail
                            meta: model.category
                            artVariant: model.artVariant
                            coverUrl: model.coverUrl
                            filePath: model.filePath
                            onPlayRequested: function(path) {
                                audioController.openFile(path)
                            }
                        }
                    }
                }

                Column {
                    width: parent.width - Math.min(640, parent.width * 0.58) - parent.spacing
                    spacing: 10

                    Text {
                        text: "Search Filters"
                        color: theme.text
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                    }

                    Repeater {
                        model: searchModels.filters

                        Rectangle {
                            width: parent.width
                            height: 36
                            radius: 8
                            color: "#ffffff"
                            border.color: theme.line

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 14
                                anchors.verticalCenter: parent.verticalCenter
                                text: model.title
                                color: theme.text
                                font.pixelSize: 13
                                font.weight: Font.DemiBold
                            }
                        }
                    }
                }
            }

            Item { width: 1; height: 148 }
        }
    }
}
