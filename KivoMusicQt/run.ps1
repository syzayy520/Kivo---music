$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $ProjectRoot "build\Release"
$QtDir = Join-Path $ProjectRoot ".qt\6.8.3\msvc2022_64"
$ExePath = Join-Path $BuildDir "KivoMusicQt.exe"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Kivo Music Qt - Test Launch" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Build directory: $BuildDir" -ForegroundColor Yellow
Write-Host "Qt directory: $QtDir" -ForegroundColor Yellow
Write-Host ""

if (-not (Test-Path -LiteralPath $ExePath)) {
    throw "Executable not found: $ExePath"
}

$env:PATH = (Join-Path $QtDir "bin") + ";$env:PATH"
$env:QT_PLUGIN_PATH = Join-Path $QtDir "plugins"
$env:QML_IMPORT_PATH = Join-Path $QtDir "qml"
$env:QML2_IMPORT_PATH = $env:QML_IMPORT_PATH

Write-Host "Files in Release directory:" -ForegroundColor Green
Get-ChildItem -LiteralPath $BuildDir -Include *.exe,*.dll -File | Select-Object Name, Length | Format-Table
Write-Host ""

Write-Host "Launching KivoMusicQt.exe..." -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
& $ExePath
