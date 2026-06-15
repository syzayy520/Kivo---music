import QtQuick
import QtQuick.Controls
import "../../components/content"
import "../../components/common"
import "../../tokens"

Item {
    id: root

    Theme { id: theme }

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
            spacing: 22

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
                title: "Recently Added"
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels ? libraryModels.recent : null
            }

            AlbumShelfSection {
                title: "Rediscover"
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels ? libraryModels.rediscover : null
            }

            AlbumShelfSection {
                title: "After Hours"
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels ? libraryModels.afterHours : null
            }

            Item {
                width: 1
                height: 76
            }
        }
    }
}
