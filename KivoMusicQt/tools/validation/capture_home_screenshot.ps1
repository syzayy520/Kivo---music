# =============================================================================
# capture_home_screenshot.ps1
# KivoMusicQt real Qt screenshot validation entry
# =============================================================================

param(
    [string]$ProjectRoot,
    [string]$BuildDir,
    [string]$OutputPath,
    [string]$PageKey
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

if (-not $BuildDir) {
    $BuildDir = Join-Path $ProjectRoot "build"
}
$BuildDir = [System.IO.Path]::GetFullPath($BuildDir)

if (-not $OutputPath) {
    $OutputPath = Join-Path $ProjectRoot "out\screenshots\home.png"
}
$OutputPath = [System.IO.Path]::GetFullPath($OutputPath)

Write-Host "=== KivoMusicQt Screenshot Capture ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "BUILD_DIR: $BuildDir"
Write-Host "OUTPUT_PATH: $OutputPath"
if ($PageKey) {
    Write-Host "PAGE_KEY: $PageKey"
}
Write-Host ""

$exePath = Join-Path $BuildDir "KivoMusicQt.exe"
if (-not (Test-Path -LiteralPath $exePath)) {
    Write-Host "Executable not found: $exePath"
    Write-Host "CLASSIFICATION: BLOCKED_QT_FRONTEND_BUILD_REQUIRED"
    exit 1
}

$localQtBin = Join-Path $ProjectRoot ".qt\6.8.3\msvc2022_64\bin"
if (Test-Path -LiteralPath $localQtBin) {
    $env:PATH = "$localQtBin;$env:PATH"
    $env:QML_IMPORT_PATH = Join-Path $ProjectRoot ".qt\6.8.3\msvc2022_64\qml"
}

$outputDirectory = Split-Path -Parent $OutputPath
New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null

if ($PageKey) {
    & $exePath --screenshot $OutputPath --screenshot-page $PageKey
} else {
    & $exePath --screenshot $OutputPath
}
if ($LASTEXITCODE -ne 0) {
    Write-Host "Screenshot capture failed with exit code $LASTEXITCODE"
    Write-Host "CLASSIFICATION: FAIL_QT_FRONTEND_SCREENSHOT_CAPTURE"
    exit 1
}

if (-not (Test-Path -LiteralPath $OutputPath)) {
    Write-Host "Screenshot output missing: $OutputPath"
    Write-Host "CLASSIFICATION: FAIL_QT_FRONTEND_SCREENSHOT_MISSING"
    exit 1
}

$file = Get-Item -LiteralPath $OutputPath
if ($file.Length -le 0) {
    Write-Host "Screenshot output is empty: $OutputPath"
    Write-Host "CLASSIFICATION: FAIL_QT_FRONTEND_SCREENSHOT_EMPTY"
    exit 1
}

Write-Host "PASS"
Write-Host "Screenshot bytes: $($file.Length)"
Write-Host "CLASSIFICATION: PASS_QT_FRONTEND_SCREENSHOT_CAPTURE"
exit 0
