# Apple Music Reference Competitive Audit

Ticket: KIVO-QT-UI-POLISH-APPLE-MUSIC-002
Status: Active design control

## What Apple Music Gets Right

- The app is calm before it is decorative: a white content field, soft sidebar, and low-noise controls.
- Navigation is semantic and instantly recognizable: Home, New, Radio, Artists, Albums, Songs, and playlists all use music-native signs.
- The home page sells music through content density. Editorial cards and album covers carry the page instead of chrome.
- The player reads as a translucent system control. It floats, stays short, and does not become a technical toolbar.
- Album art is the main color system. The shell stays neutral so covers can feel alive.
- Metadata is restrained. Codec and device state never overpower the music surface.

## What Kivo Must Not Copy

- Do not copy Apple branding, exact card text, exact icons, or exact proportions.
- Do not make Kivo a skin of Apple Music. Kivo is a high-fidelity local, NAS, and device-aware player.
- Do not hide audio truth, but do not advertise it with loud badges in the primary chrome.
- Do not use generic template art that reads as placeholder content.

## How Kivo Can Surpass The Reference

- Make local ownership feel premium: imported, NAS, and local library state should be quiet but trustworthy.
- Make lossless playback feel serious without hardware-promo badges.
- Keep the desktop glass lighter and more precise than a web player bar.
- Keep the information architecture sharper for power listeners while preserving a music-first first impression.
- Use Kivo's own editorial language: Signal Mix, Album Focus, Night Drive, and library-aware recommendations.

## Kivo Visual North Star

Kivo should feel like a premium native music instrument:

- cool white and system gray shell,
- vivid release-like artwork,
- short floating glass player,
- semantic music navigation,
- calm high-fidelity status,
- no loud marketing badges,
- no admin-dashboard density,
- no fake split hero banner.

## Natural Family Tree

```text
ui/qml
|-- tokens
|   `-- Theme.qml                    # shared color, size, and surface tokens
|-- components
|   |-- artwork
|   |   `-- AlbumArtwork.qml         # release-like artwork generation only
|   |-- content
|   |   |-- TopPickCard.qml          # one editorial music card
|   |   |-- HomeRecommendationSection.qml
|   |   |-- ContinueListeningSection.qml
|   |   |-- ContinueListeningCard.qml
|   |   |-- LibraryAddedSection.qml
|   |   |-- RecentListeningSection.qml
|   |   `-- RecentListenStrip.qml
|   |-- navigation
|   |   |-- Sidebar.qml              # native shell navigation composition
|   |   |-- NavigationItem.qml       # one navigation row
|   |   `-- NavigationGlyph.qml      # semantic icon drawing
|   |-- shell
|   |   `-- TopBar.qml               # page title, navigation arrows, search, avatar
|   `-- transport
|       |-- TransportBar.qml         # floating player container
|       |-- PlaybackControlCluster.qml
|       |-- TransportTrackInfo.qml
|       |-- AudioStatusText.qml
|       `-- VolumeCluster.qml
`-- windows
    `-- HomeWindow.qml               # page composition only
```

## Current Round Decisions

- Replace the old single large hero banner with a Kivo editorial card rail.
- Remove Apple-like marks from Kivo cards; Kivo identity must be original and restrained.
- Keep the sidebar English for now to avoid mixed-language chrome and to match native desktop rhythm.
- Remove unused hero-specific files after the card rail fully replaces the hero.
- Keep `FLAC 24-bit / 96kHz` as a single quiet player status.
- Keep deep audio truth available in the product, but do not place a technical evidence panel in the home page first impression.
