# =============================================================================
# check_qt_environment.ps1
# KivoMusicQt local Qt toolchain probe
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== KivoMusicQt Qt Environment Probe ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$missing = @()

function Test-CommandAvailable {
    param([Parameter(Mandatory = $true)][string]$Name)
    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if ($command) {
        Write-Host ("{0}: {1}" -f $Name, $command.Source)
        return $true
    }
    Write-Host ("{0}: MISSING" -f $Name)
    return $false
}

if (-not (Test-CommandAvailable -Name "cmake")) {
    $missing += "cmake"
}
if (-not (Test-CommandAvailable -Name "ninja")) {
    $vsNinja = "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
    if (Test-Path -LiteralPath $vsNinja) {
        Write-Host "ninja(vs): $vsNinja"
    } else {
        $missing += "ninja"
    }
}
if (-not (Test-CommandAvailable -Name "cl")) {
    $vsCl = Get-ChildItem `
        -Path "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC" `
        -Recurse `
        -Filter "cl.exe" `
        -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -like "*\bin\Hostx64\x64\cl.exe" } |
        Select-Object -First 1
    if ($vsCl) {
        Write-Host "cl(vs): $($vsCl.FullName)"
    } else {
        $missing += "cl"
    }
}
if (-not (Test-CommandAvailable -Name "qt-cmake")) {
    $localQtCmake = Join-Path $ProjectRoot ".qt\6.8.3\msvc2022_64\bin\qt-cmake.bat"
    if (Test-Path -LiteralPath $localQtCmake) {
        Write-Host "qt-cmake(local): $localQtCmake"
    } else {
        $missing += "qt-cmake"
    }
}
if (-not (Test-CommandAvailable -Name "qmltestrunner")) {
    $localQmlTestRunner = Join-Path $ProjectRoot ".qt\6.8.3\msvc2022_64\bin\qmltestrunner.exe"
    if (Test-Path -LiteralPath $localQmlTestRunner) {
        Write-Host "qmltestrunner(local): $localQmlTestRunner"
    } else {
        $missing += "qmltestrunner"
    }
}

Write-Host ""

if ($missing.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "CLASSIFICATION: PASS_QT_FRONTEND_ENVIRONMENT"
    exit 0
}

Write-Host "BLOCKED"
Write-Host "Missing local prerequisites: $($missing -join ', ')"
Write-Host "CLASSIFICATION: BLOCKED_QT_FRONTEND_ENVIRONMENT"
exit 1
