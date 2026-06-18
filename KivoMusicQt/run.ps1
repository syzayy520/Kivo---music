$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path

# Qt location: KIVO_QT_ROOT env var first, then the project-local 6.8 LTS install.
$LocalQt = Join-Path $ProjectRoot ".qt\6.8.3\msvc2022_64"
if ($env:KIVO_QT_ROOT -and (Test-Path -LiteralPath $env:KIVO_QT_ROOT)) {
    $QtDir = $env:KIVO_QT_ROOT
} elseif (Test-Path -LiteralPath $LocalQt) {
    $QtDir = $LocalQt
} else {
    throw "Qt6 not found. Set KIVO_QT_ROOT or install Qt 6.8.3 to $LocalQt."
}

# Locate the built executable across the supported build layouts.
$Candidates = @(
    (Join-Path $ProjectRoot "out\local-build\Release\KivoMusicQt.exe"),
    (Join-Path $ProjectRoot "out\local-build\KivoMusicQt.exe"),
    (Join-Path $ProjectRoot "build\Release\KivoMusicQt.exe")
)
$ExePath = $Candidates | Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1
if (-not $ExePath) {
    throw "Executable not found. Build first, e.g.:`n  `$env:KIVO_QT_ROOT='$QtDir'`n  cmake --build out\local-build --config Release"
}
$BuildDir = Split-Path -Parent $ExePath

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Kivo Music Qt - Test Launch" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Qt directory: $QtDir" -ForegroundColor Yellow
Write-Host "Executable:   $ExePath" -ForegroundColor Yellow
Write-Host ""

$env:PATH = (Join-Path $QtDir "bin") + ";$env:PATH"
$env:QT_PLUGIN_PATH = Join-Path $QtDir "plugins"
$env:QML_IMPORT_PATH = Join-Path $QtDir "qml"
$env:QML2_IMPORT_PATH = $env:QML_IMPORT_PATH

Write-Host "Launching KivoMusicQt.exe..." -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
& $ExePath
