# =============================================================================
# build_qt_frontend.ps1
# KivoMusicQt configure/build validation entry
# =============================================================================

param(
    [string]$ProjectRoot,
    [string]$BuildDir
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

Write-Host "=== KivoMusicQt Build Validation ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "BUILD_DIR: $BuildDir"
Write-Host ""

$environment = & (Join-Path $PSScriptRoot "invoke_vs_qt_environment.ps1") `
    -ProjectRoot $ProjectRoot
Write-Host "QtRoot: $($environment.QtRoot)"
Write-Host "NinjaPath: $($environment.NinjaPath)"
Write-Host ""

& (Join-Path $environment.QtBin "qt-cmake.bat") `
    -S $ProjectRoot `
    -B $BuildDir `
    -G Ninja
if ($LASTEXITCODE -ne 0) {
    Write-Host "CLASSIFICATION: FAIL_QT_FRONTEND_CONFIGURE"
    exit 1
}

$buildStdout = Join-Path $BuildDir "kivo_build_stdout.log"
$buildStderr = Join-Path $BuildDir "kivo_build_stderr.log"
$cmakePath = (Get-Command cmake).Source
$buildProcess = Start-Process `
    -FilePath $cmakePath `
    -ArgumentList @("--build", $BuildDir) `
    -WindowStyle Hidden `
    -Wait `
    -PassThru `
    -RedirectStandardOutput $buildStdout `
    -RedirectStandardError $buildStderr

$buildOutput = @()
if (Test-Path -LiteralPath $buildStdout) {
    $buildOutput += Get-Content -LiteralPath $buildStdout
}
if (Test-Path -LiteralPath $buildStderr) {
    $buildOutput += Get-Content -LiteralPath $buildStderr
}

foreach ($line in $buildOutput) {
    $text = [string]$line
    if ($text -match "包含文件|including file") {
        continue
    }
    Write-Host $text
}
if ($buildProcess.ExitCode -ne 0) {
    Write-Host "CLASSIFICATION: FAIL_QT_FRONTEND_BUILD"
    exit 1
}

Write-Host "PASS"
Write-Host "CLASSIFICATION: PASS_QT_FRONTEND_BUILD"
exit 0
