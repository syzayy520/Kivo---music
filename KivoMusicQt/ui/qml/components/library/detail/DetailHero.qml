// =============================================================================
// Kivo Music - DetailHero.qml
// 职责: 详情页头部 —— 大封面 + 类目 + 标题 + 动作条。纯布局,只组装与转发信号。
// circle=true 用于艺人(圆形头像),false 用于专辑(圆角方形)。
// =============================================================================

import QtQuick
import QtQuick.Effects
import "../../artwork"
import "."
import KivoMusic

Item {
    id: root
    property string eyebrow: ""      // "ARTIST" / "ALBUM"
    property string title: ""
    property url coverUrl: ""
    property int artVariant: 0
    property bool circle: false
    signal playClicked()
    signal shuffleClicked()

    implicitHeight: 188

    AlbumArtwork {
        id: art
        width: 176; height: 176
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        radiusValue: root.circle ? width / 2 : 14
        sourceUrl: root.coverUrl
        variant: root.artVariant

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Theme.shadowModal
            shadowBlur: 0.8
            shadowVerticalOffset: 14
        }
    }

    Column {
        anchors.left: art.right
        anchors.leftMargin: 28
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 14

        Text {
            width: parent.width
            text: root.eyebrow
            color: Theme.muted
            font.pixelSize: 11
            font.weight: Font.DemiBold
            font.letterSpacing: 0.8
            font.capitalization: Font.AllUppercase
            font.family: Theme.fontFamily
            elide: Text.ElideRight
        }
        Text {
            width: parent.width
            text: root.title
            color: Theme.text
            font.pixelSize: 34
            font.weight: Font.Bold
            font.letterSpacing: -0.5
            font.family: Theme.fontFamily
            elide: Text.ElideRight
            maximumLineCount: 2
            wrapMode: Text.WordWrap
        }
        DetailActionBar {
            onPlayClicked: root.playClicked()
            onShuffleClicked: root.shuffleClicked()
        }
    }
}
