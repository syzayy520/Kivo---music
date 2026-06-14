import QtQuick
import QtQuick.Controls
import "../../components/content"
import "../../tokens"

Item {
    id: root

    Theme { id: theme }

    ScrollView {
        id: scroll
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth

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
                contentModel: libraryModels.topPicks
            }

            ContinueListeningSection {
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels.continueListening
            }

            AlbumShelfSection {
                title: "Recently Added"
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels.recent
            }

            AlbumShelfSection {
                title: "Rediscover"
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels.rediscover
            }

            AlbumShelfSection {
                title: "After Hours"
                x: homeColumn.contentMargin
                width: homeColumn.contentWidth
                contentWidth: width
                contentModel: libraryModels.afterHours
            }

            Item {
                width: 1
                height: 76
            }
        }
    }
}
