// =============================================================================
// Kivo Music - Theme.qml
// Apple Music-level design token system with dark mode support
// =============================================================================

import QtQuick

QtObject {
    // ── Mode ──────────────────────────────────────────────────
    property bool isDark: false

    function toggle() {
        isDark = !isDark;
    }

    // ── Color System (resolved by mode) ───────────────────────
    // Background
    property color page: isDark ? "#1c1c1e" : "#f5f5f7"
    property color panel: isDark ? "#2c2c2e" : "#ffffff"
    property color panelSoft: isDark ? "#242426" : "#f2f2f4"
    property color panelHover: isDark ? "#3a3a3c" : "#fafafa"

    // Text
    property color text: isDark ? "#f5f5f7" : "#1d1d1f"
    property color textSecondary: isDark ? "#aeaeb2" : "#494949"
    property color muted: isDark ? "#aeaeb2" : "#6e6e73"
    property color faint: isDark ? "#636366" : "#aeaeb2"

    // Accent (same in both modes)
    readonly property color accent: "#0071e3"
    readonly property color accentPink: "#fa233b"
    readonly property color accentHot: "#ff3b30"
    readonly property color accentGreen: "#34c759"
    readonly property color accentGold: "#ff9f0a"

    // Line & Border
    property color line: isDark ? "#38383a" : "#e5e5ea"
    property color lineSubtle: isDark ? "#2c2c2e" : "#f0f0f2"

    // Transport specific
    property color transportFill: isDark ? "#f5f5f7" : "#1c1c1e"
    property color transportTrack: isDark ? "#48484a" : "#e0e0e2"
    property color transportHover: isDark ? "#3a3a3c" : "#f0f0f2"

    // ── Typography ────────────────────────────────────────────
    readonly property string fontFamily: "Segoe UI"
    readonly property string fontFamilyMono: "Cascadia Code"

    // ── Radius ────────────────────────────────────────────────
    readonly property int radiusXS: 4
    readonly property int radiusSmall: 6
    readonly property int radiusMedium: 8
    readonly property int radiusLarge: 10
    readonly property int radiusXL: 14
    readonly property int radiusXXL: 20
    readonly property int radiusRound: 999

    // ── Spacing (8pt grid) ────────────────────────────────────
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

    // ── Layout ────────────────────────────────────────────────
    readonly property int railWidth: 220
    readonly property int railWidthCollapsed: 64
    readonly property int transportHeight: 64
    readonly property int topBarHeight: 56
    readonly property int hitSize: 44
    readonly property int hitSizeSmall: 36

    // ── Animation ─────────────────────────────────────────────
    readonly property int animFast: 120
    readonly property int animNormal: 200
    readonly property int animSlow: 350
    readonly property int animSpring: 500

    // ── Shadow ────────────────────────────────────────────────
    property string shadowCard: isDark ? "#0cffffff" : "#08000000"
    property string shadowPanel: isDark ? "#18ffffff" : "#12000000"
    property string shadowModal: isDark ? "#2affffff" : "#20000000"
    property string shadowPopover: isDark ? "#20ffffff" : "#1a000000"

    // ── Z-index layers ────────────────────────────────────────
    readonly property int zContent: 1
    readonly property int zOverlay: 10
    readonly property int zDropdown: 20
    readonly property int zModal: 30
    readonly property int zTooltip: 40
    readonly property int zToast: 50
}
