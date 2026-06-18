// =============================================================================
// Kivo Music - Theme.qml
// Apple Music-level design token system with dark mode support.
// SINGLETON: exactly one instance app-wide, so theme.toggle()/isDark is global.
// Access from QML as `import KivoMusic` then `Theme.text` etc. (no instantiation).
// =============================================================================

pragma Singleton

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

    // Sidebar chrome — subtly distinct from `page` (recessed in dark, frosted-light
    // in light), with a hairline right edge instead of a heavy drop shadow.
    property color sidebar: isDark ? "#161618" : "#fbfbfd"
    property color sidebarBorder: isDark ? "#2c2c2e" : "#e8e8ec"

    // Content cards (Pick Up Listening, etc.) — translucent over the page in dark
    // mode so they never glare white; solid white in light mode.
    property color card: isDark ? "#14ffffff" : "#ffffff"
    property color cardBorder: isDark ? "#1fffffff" : "#e7e3e1"
    property color cardHover: isDark ? "#1fffffff" : "#fafafa"

    // Modal/overlay scrim (queue drawer, dialogs) — mode-aware so the near-white
    // light UI isn't crushed to dark gray by a flat heavy black dim.
    property color scrim: isDark ? "#99000000" : "#40000000"

    // Text
    property color text: isDark ? "#f5f5f7" : "#1d1d1f"
    property color textSecondary: isDark ? "#aeaeb2" : "#494949"
    property color muted: isDark ? "#aeaeb2" : "#6e6e73"
    property color faint: isDark ? "#636366" : "#aeaeb2"

    // Accent — Kivo's signature warm gold (the "K" brand mark), our identity vs
    // Apple Music's pink / Spotify's green. One vivid gold for fills/icons/progress;
    // a mode-aware readable variant for accent-COLORED TEXT (gold-on-white needs to
    // be darker); a dark on-accent for text atop a SOLID gold fill; a faint wash
    // for selection/hover highlights.
    readonly property color accent: "#e0a82e"
    property color accentText: isDark ? "#e8b84b" : "#9a6b00"
    // Text/glyph color ON a solid gold fill (dark, for contrast). NB: cannot be
    // named with an "on" prefix — QML would parse it as a signal handler.
    readonly property color accentInk: "#1d1d1f"
    property color accentSoft: isDark ? "#33e0a82e" : "#1fe0a82e"
    readonly property color accentPink: "#fa233b"
    readonly property color accentHot: "#ff3b30"
    readonly property color accentGreen: "#34c759"
    readonly property color accentGold: "#ff9f0a"

    // Brand "ink" — the dark tile behind the gold "K" mark; dark in both modes.
    readonly property color ink: isDark ? "#0a0a0a" : "#1d1d1f"
    // Brand "K" glyph color — softer gold on the ink tile for contrast without
    // being too vivid; mode-invariant because the tile is always dark.
    readonly property color brandK: "#f4d06f"

    // Line & Border
    property color line: isDark ? "#38383a" : "#e5e5ea"
    property color lineSubtle: isDark ? "#2c2c2e" : "#f0f0f2"

    // Transport specific
    property color transportFill: isDark ? "#f5f5f7" : "#1c1c1e"
    property color transportTrack: isDark ? "#48484a" : "#e0e0e2"
    property color transportHover: isDark ? "#3a3a3c" : "#f0f0f2"
    // Frosted-glass capsule fill + hairline border, per mode (8-digit #AARRGGBB).
    // Translucent so the blurred backdrop reads through it, like Apple Music.
    property color transportGlass: isDark ? "#e61f1f22" : "#f2fbfbfd"
    property color transportGlassBorder: isDark ? "#1affffff" : "#16000000"
    property color transportTopEdge: isDark ? "#14ffffff" : "#ccffffff"

    // ── Typography ────────────────────────────────────────────
    readonly property string fontFamily: "Segoe UI"
    readonly property string fontFamilyMono: "Cascadia Code"

    // ── Type scale ────────────────────────────────────────────
    // A deliberate 3-tier rhythm so the eye can SCAN: a heavy section title, a
    // whisper-quiet section subtitle (40% — the third tier), then card text.
    // Apple Music's calm comes from this contrast, not from more chrome.
    readonly property int fontHero:        28   // page hero
    readonly property int fontSectionTitle: 21  // shelf / section heading (Bold)
    readonly property int fontCardTitle:    13   // album/track title on a card
    readonly property int fontSubtitle:     12   // section subtitle / card subtitle
    readonly property int fontCaption:      11   // eyebrow / metadata / footnote
    // Opacity for the quiet third tier (section subtitles, footnotes).
    readonly property real subtitleOpacity: 0.4

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
    readonly property int railWidth: 208
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
    property color shadowCard: isDark ? "#0cffffff" : "#08000000"
    property color shadowPanel: isDark ? "#18ffffff" : "#12000000"
    property color shadowModal: isDark ? "#2affffff" : "#20000000"
    property color shadowPopover: isDark ? "#20ffffff" : "#1a000000"

    // ── Extended surface tokens (E1 补充) ────────────────────
    // 专辑封面占位背景: 无封面时的色块, 暗模式不刺眼。
    property color coverPlaceholder: isDark ? "#3a3a3c" : "#e0e0e5"
    // 通用控件悬停背景 (小圆按钮等)
    property color controlHover: isDark ? "#38383a" : "#f0f0f2"
    // Now-Playing 视图的可读性暗幕渐变站点 (always-dark, mode-invariant)
    readonly property color npScrimLight:  "#1a000000"
    readonly property color npScrimMid:   "#33000000"
    readonly property color npScrimHeavy: "#80000000"
    // Now-Playing immersive overlay text / control tokens (always-dark, mode-invariant):
    // all are white-alpha composited over a blurred dark album backdrop.
    readonly property color npText:         "#ffffff"    // primary title / fills
    readonly property color npTextCard:     "#e8eaed"    // subtitle on dark artwork card (opaque near-white)
    readonly property color npTextDim:      "#cfffffff"  // artist subtitle (81% white)
    readonly property color npTextFaint:    "#b0ffffff"  // time labels (69% white)
    readonly property color npTextMuted:    "#d8ffffff"  // metadata (85% white)
    readonly property color npSeparator:    "#66ffffff"  // dot separators (40% white)
    readonly property color npTrack:        "#40ffffff"  // scrubber track (25% white)
    readonly property color npButton:       "#ffffff"    // play button fill
    readonly property color npButtonGlyph:  "#111114"    // dark glyph on white button
    readonly property color npControlBg:    "#1fffffff"  // collapse button rest (12% white)
    readonly property color npControlHover: "#33ffffff"  // collapse button hover (20% white)
    // 焦点环 / 选中高亮 (Accessible: outline色)
    property color focusRing: isDark ? "#e0a82e" : "#0061d5"

    // ── Extended text hierarchy ───────────────────────────────────────────────
    // Placeholder / hint / footnote text — lighter than `muted`.
    property color textPlaceholder: isDark ? "#636366" : "#86868b"
    // Inactive step-indicator dot, disabled-state fills, etc.
    property color inactive: isDark ? "#48484a" : "#d0d0d5"

    // ── CTA / Dialog primary action ───────────────────────────────────────────
    // Standard platform blue for primary dialog buttons and onboarding actions.
    // Distinct from Kivo's gold accent: dialogs use platform convention.
    readonly property color ctaPrimary: "#0071e3"

    // ── Window frame controls ────────────────────────────────────────────────
    // Standard close-button red (Windows 11 / macOS convention) and generic
    // control-button hover fill.
    readonly property color winClose:       "#e5484d"
    property color winControlHover: isDark ? "#3a3a3c" : "#edf0f4"

    // ── Equalizer UI ─────────────────────────────────────────────────────────
    // Semantic colors for EQ slider gain direction (blue = boost, red = cut).
    // These match the conventional DAW color vocabulary and are mode-invariant
    // because the EQ panel renders on a solid `panel` background either way.
    readonly property color eqPositive:     "#0071e3"
    readonly property color eqNegative:     "#e84b5e"
    property color eqTrack:   isDark ? "#3a3a3c" : "#e0e0e0"
    property color eqHandle:  isDark ? "#ffffff" : "#ffffff"
    property color eqHandleBorder: isDark ? "#636366" : "#c0c0c0"

    // ── Z-index layers ────────────────────────────────────────
    readonly property int zContent: 1
    readonly property int zOverlay: 10
    readonly property int zDropdown: 20
    readonly property int zModal: 30
    readonly property int zTooltip: 40
    readonly property int zToast: 50
}
