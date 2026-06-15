// =============================================================================
// Kivo Music - Theme.qml
// Commercial white macOS / Apple Music inspired token system
// =============================================================================

import QtQuick

QtObject {
    property bool isDark: false

    function toggle() {
        isDark = !isDark;
    }

    // ── Surfaces ────────────────────────────────────────────────
    property color page: isDark ? "#161617" : "#f5f5f7"
    property color canvas: isDark ? "#1c1c1e" : "#fbfbfd"
    property color panel: isDark ? "#242426" : "#ffffff"
    property color panelSoft: isDark ? "#2c2c2e" : "#f2f2f4"
    property color panelHover: isDark ? "#333336" : "#f7f7f9"
    property color sidebar: isDark ? "#1f1f21" : "#f7f7f9"
    property color glass: isDark ? "#dc242426" : "#eafdfdfd"

    // ── Text ───────────────────────────────────────────────────
    property color text: isDark ? "#f5f5f7" : "#1d1d1f"
    property color textSecondary: isDark ? "#c7c7cc" : "#3c3c43"
    property color muted: isDark ? "#98989d" : "#6e6e73"
    property color faint: isDark ? "#636366" : "#a1a1a6"

    // ── Accent ─────────────────────────────────────────────────
    readonly property color accent: "#fa233b"
    readonly property color accentPink: "#fa233b"
    readonly property color accentHot: "#ff375f"
    readonly property color accentBlue: "#0071e3"
    readonly property color accentGreen: "#34c759"
    readonly property color accentGold: "#ff9f0a"

    // ── Lines / controls ───────────────────────────────────────
    property color line: isDark ? "#3a3a3c" : "#e5e5ea"
    property color lineSubtle: isDark ? "#2c2c2e" : "#eeeeF2"
    property color transportFill: isDark ? "#ffffff" : "#1d1d1f"
    property color transportTrack: isDark ? "#48484a" : "#d9d9df"
    property color transportHover: isDark ? "#3a3a3c" : "#ededf1"

    // ── Typography ─────────────────────────────────────────────
    readonly property string fontFamily: "Segoe UI"
    readonly property string fontFamilyMono: "Cascadia Code"

    // ── Radius ─────────────────────────────────────────────────
    readonly property int radiusXS: 5
    readonly property int radiusSmall: 7
    readonly property int radiusMedium: 10
    readonly property int radiusLarge: 14
    readonly property int radiusXL: 18
    readonly property int radiusXXL: 26
    readonly property int radiusRound: 999

    // ── Spacing ────────────────────────────────────────────────
    readonly property int space1: 4
    readonly property int space2: 8
    readonly property int space3: 12
    readonly property int space4: 16
    readonly property int space5: 20
    readonly property int space6: 24
    readonly property int space8: 32
    readonly property int space10: 40
    readonly property int space12: 48
    readonly property int space16: 64

    // ── Layout ─────────────────────────────────────────────────
    readonly property int railWidth: 232
    readonly property int railWidthCollapsed: 68
    readonly property int transportHeight: 82
    readonly property int topBarHeight: 60
    readonly property int hitSize: 44
    readonly property int hitSizeSmall: 36

    // ── Animation ──────────────────────────────────────────────
    readonly property int animFast: 110
    readonly property int animNormal: 190
    readonly property int animSlow: 320
    readonly property int animSpring: 460

    // ── Shadow ─────────────────────────────────────────────────
    property string shadowCard: isDark ? "#22000000" : "#09000000"
    property string shadowPanel: isDark ? "#30000000" : "#14000000"
    property string shadowModal: isDark ? "#44000000" : "#24000000"
    property string shadowPopover: isDark ? "#44000000" : "#1c000000"

    // ── Z layers ───────────────────────────────────────────────
    readonly property int zContent: 1
    readonly property int zOverlay: 10
    readonly property int zDropdown: 20
    readonly property int zModal: 30
    readonly property int zTooltip: 40
    readonly property int zToast: 50
}
