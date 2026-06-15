# KivoMusicQt

Qt 6 desktop frontend prototype for Kivo Music.

This product frontend is intentionally outside `KivoAudioCoreCpp` so AudioCore
remains UI-free. The first slice is a polished static shell that mirrors the
current design direction and prepares the natural Qt/QML family tree for later
real Host ABI integration.

Architecture baseline:

- `docs/architecture/kivo_desktop_commercial_architecture.md`
- `docs/architecture/family_gate.md`
- `docs/design/home_iteration_001.md`

Local probes:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\gates\check_qt_family_gate.ps1 -ProjectRoot .
powershell -ExecutionPolicy Bypass -File .\tools\validation\check_qt_environment.ps1 -ProjectRoot .
powershell -ExecutionPolicy Bypass -File .\tools\validation\build_qt_frontend.ps1 -ProjectRoot .
powershell -ExecutionPolicy Bypass -File .\tools\validation\build_qt_frontend_ascii_mirror.ps1 -ProjectRoot .
powershell -ExecutionPolicy Bypass -File .\tools\validation\capture_home_screenshot.ps1 -ProjectRoot .
```
