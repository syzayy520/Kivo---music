# =============================================================================
# build_qt_frontend_ascii_mirror.ps1
# KivoMusicQt build validation through an ASCII source mirror
# =============================================================================

param(
    [string]$ProjectRoot,
    [string]$MirrorRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

if (-not $MirrorRoot) {
    $MirrorRoot = "C:\tmp\KivoMusicQt_ascii"
}
$MirrorRoot = [System.IO.Path]::GetFullPath($MirrorRoot)

if (-not $MirrorRoot.StartsWith("C:\tmp\", [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "MirrorRoot must stay under C:\tmp"
}

Write-Host "=== KivoMusicQt ASCII Mirror Build ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "MIRROR_ROOT: $MirrorRoot"
Write-Host ""

if (Test-Path -LiteralPath $MirrorRoot) {
    Remove-Item -LiteralPath $MirrorRoot -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $MirrorRoot | Out-Null

$excludeDirectories = @(".qt", "build", "out", ".git")
$sourceItems = Get-ChildItem -LiteralPath $ProjectRoot -Force |
    Where-Object { $excludeDirectories -notcontains $_.Name }
foreach ($item in $sourceItems) {
    Copy-Item -LiteralPath $item.FullName `
        -Destination (Join-Path $MirrorRoot $item.Name) `
        -Recurse `
        -Force
}

$qtSource = Join-Path $ProjectRoot ".qt\6.8.3\msvc2022_64"
$qtMirror = Join-Path $MirrorRoot ".qt\6.8.3\msvc2022_64"
if (-not (Test-Path -LiteralPath $qtSource)) {
    throw "Qt kit missing: $qtSource"
}
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $qtMirror) | Out-Null
Copy-Item -LiteralPath $qtSource -Destination $qtMirror -Recurse -Force

& (Join-Path $MirrorRoot "tools\validation\build_qt_frontend.ps1") `
    -ProjectRoot $MirrorRoot `
    -BuildDir (Join-Path $MirrorRoot "build")
if ($LASTEXITCODE -ne 0) {
    Write-Host "CLASSIFICATION: FAIL_QT_FRONTEND_ASCII_MIRROR_BUILD"
    exit 1
}

Write-Host "PASS"
Write-Host "CLASSIFICATION: PASS_QT_FRONTEND_ASCII_MIRROR_BUILD"
exit 0
