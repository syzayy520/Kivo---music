# Kivo Music — Store Listing Content

## Short Description (100 chars)
Kivo Music — A beautiful desktop music player with bit-perfect audio and Apple Music-level design.

## Description (1500 chars)
Kivo Music is a premium desktop music player designed for audiophiles and music lovers. Built with a C++20 audio engine and Qt6/QML interface, it delivers bit-perfect WASAPI playback with Apple Music-level visual polish.

### Key Features:
- 🎵 Bit-Perfect Audio — WASAPI exclusive mode for pristine sound
- 🎨 Apple Music-Level Design — Frosted glass, smooth animations, dark mode
- 📂 Local Library — Scan and organize your music collection
- 🔄 Playback Queue — Sequential, shuffle, repeat modes
- 🎤 Lyrics Display — LRC file support with live synchronization
- 🖼️ Album Artwork — Automatic cover art fetching from MusicBrainz
- ⌨️ Keyboard Shortcuts — Full keyboard control for power users
- 🌓 Dark Mode — Beautiful light and dark themes
- 📋 Playlist Management — Create, edit, and organize playlists
- 🎚️ Audio Visualizer — Live spectrum bars during playback

### Supported Formats:
FLAC, MP3, WAV, AAC, OGG, and more — powered by FFmpeg.

### Privacy:
Kivo Music is a local-first application. No personal data is collected, stored, or transmitted. All data stays on your device.

## Keywords
music player, audio player, flac player, bit perfect, audiophile, wasapi, local music, playlist, lossless audio

## Screenshots (required)
At least 4 screenshots at 1366×768 resolution:
1. Home page with album grid
2. Now playing with transport bar
3. Library browser
4. Settings page

## Store Listing Requirements Checklist
- [x] App name: Kivo Music
- [x] Short description
- [x] Full description
- [x] Keywords
- [ ] Screenshots (4+ at 1366×768)
- [x] Privacy policy URL (in-app page)
- [x] EULA (in-app page)
- [ ] Age rating questionnaire
- [ ] Export compliance
- [ ] Pricing: Free
- [ ] Category: Music

## Visual Asset Specifications
All assets should be placed in `store/Assets/`:

| Asset | Size | Format |
|-------|------|--------|
| StoreLogo.png | 50×50 | PNG |
| Square44x44Logo.png | 44×44 | PNG |
| Square150x150Logo.png | 150×150 | PNG |
| Wide310x150Logo.png | 310×150 | PNG |
| LargeTile.png | 310×310 | PNG |
| SplashScreen.png | 620×300 | PNG |
| Store Listing Screenshot 1 | 1366×768 | PNG |
| Store Listing Screenshot 2 | 1366×768 | PNG |
| Store Listing Screenshot 3 | 1366×768 | PNG |
| Store Listing Screenshot 4 | 1366×768 | PNG |

## Build & Package
```powershell
# Build the app
msbuild KivoMusicQt.vcxproj /p:Configuration=Release /p:Platform=x64

# Create MSIX package (requires Windows SDK)
makeappx pack /d "build\Release" /p "KivoMusic_1.0.0.0_x64.msix"

# Sign the package (requires code signing certificate)
signtool sign /fd SHA256 /a /f KivoMusic.pfx KivoMusic_1.0.0.0_x64.msix
```
