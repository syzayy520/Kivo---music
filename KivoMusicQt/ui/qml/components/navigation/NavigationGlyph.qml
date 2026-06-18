// =============================================================================
// Kivo Music - NavigationGlyph.qml
// 职责: 按 kind 选用 glyphs/ 家族里对应的导航字形,并决定颜色/粗细。本文件不含几何。
// 取代了之前一个 Canvas 画 16 种图标的巨型 switch。
// 选中=金色(与侧栏选中身份一致),否则主题感知中性灰 —— 旧实现用 Theme.ink(近黑)
// 做选中色,在暗色侧栏几乎不可见,一并修正。
// =============================================================================

import QtQuick
import "glyphs"
import KivoMusic

Item {
    id: root
    property string kind: "home"
    property bool active: false

    readonly property color glyphColor: active ? Theme.accent : Theme.muted
    readonly property real glyphWeight: active ? 1.55 : 1.35

    Loader {
        anchors.fill: parent
        sourceComponent: {
            switch (root.kind) {
            case "home": return cHome;
            case "search": return cSearch;
            case "recent": return cRecent;
            case "artist": return cArtist;
            case "album": return cAlbum;
            case "song": return cSong;
            case "favorite": return cFavorite;
            case "settings": return cSettings;
            default: return cHome;
            }
        }
    }

    Component { id: cHome;     HomeGlyph     { glyphColor: root.glyphColor; weight: root.glyphWeight } }
    Component { id: cSearch;   SearchGlyph   { glyphColor: root.glyphColor; weight: root.glyphWeight } }
    Component { id: cRecent;   RecentGlyph   { glyphColor: root.glyphColor; weight: root.glyphWeight } }
    Component { id: cArtist;   ArtistGlyph   { glyphColor: root.glyphColor; weight: root.glyphWeight } }
    Component { id: cAlbum;    AlbumGlyph    { glyphColor: root.glyphColor; weight: root.glyphWeight } }
    Component { id: cSong;     SongGlyph     { glyphColor: root.glyphColor; weight: root.glyphWeight } }
    Component { id: cFavorite; FavoriteGlyph { glyphColor: root.glyphColor; weight: root.glyphWeight } }
    Component { id: cSettings; SettingsGlyph { glyphColor: root.glyphColor; weight: root.glyphWeight } }
}
