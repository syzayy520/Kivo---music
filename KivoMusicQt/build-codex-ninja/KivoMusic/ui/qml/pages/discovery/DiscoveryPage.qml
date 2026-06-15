import QtQuick
import QtQuick.Controls
import "../../components/discovery"
import "../../tokens"

Item {
    id: root
    property string mode: "new"

    readonly property bool radioMode: mode === "radio"
    readonly property string summaryText: radioMode
        ? "Stations built from your local listening history"
        : mode === "made"
            ? "Library-aware mixes for focused listening"
            : "New albums and scans from local and NAS sources"

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
            spacing: 20

            Text {
                x: pageColumn.contentMargin
                width: pageColumn.contentWidth
                text: root.summaryText
                color: theme.text
                font.pixelSize: 14
                font.weight: Font.DemiBold
            }

            Grid {
                x: pageColumn.contentMargin
                width: pageColumn.contentWidth
                columns: 5
                columnSpacing: 24
                rowSpacing: 26

                Repeater {
                    model: libraryModels.recent

                    DiscoveryCard {
                        width: (parent.width - 96) / 5
                        eyebrow: root.radioMode ? "STATION" : model.note
                        title: model.title
                        subtitle: model.subtitle
                        artVariant: model.artVariant
                        coverUrl: model.coverUrl
                    }
                }
            }

            Item { width: 1; height: 148 }
        }
    }
}
