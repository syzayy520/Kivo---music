import QtQuick
import QtQuick.Controls
import "../../components/content"
import "../../components/common"
import KivoMusic

Item {
    id: root


    ScrollView {
        id: scroll
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth

        // 使用自定义滚动条
        ScrollBar.vertical: CustomScrollBar {
            policy: ScrollBar.AsNeeded
        }

        Column {
            id: homeColumn
            property int contentMargin: 36
            readonly property real contentWidth: width - contentMargin * 2

            width: scroll.availableWidth
            spacing: 30

            HomeRecommendationSection {
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels ? libraryModels.topPicks : null
            }

            ContinueListeningSection {
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels ? libraryModels.continueListening : null
            }

            AlbumShelfSection {
                title: qsTr("Recently Added")
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels ? libraryModels.recent : null
            }

            AlbumShelfSection {
                title: qsTr("Rediscover")
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels ? libraryModels.rediscover : null
            }

            AlbumShelfSection {
                title: qsTr("Not Heard in a While")
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels ? libraryModels.afterHours : null
            }

            // 安全留白:浮动播放条(≈ transportHeight + 浮动外边距)会盖住底部内容,
            // 这里预留 = 播放条高度 + 余量,确保最后一行专辑不被遮挡。
            Item {
                width: 1
                height: Theme.transportHeight + 72
            }
        }
    }
}
