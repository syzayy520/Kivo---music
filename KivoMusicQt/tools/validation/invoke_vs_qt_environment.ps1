# =============================================================================
# invoke_vs_qt_environment.ps1
# Shared helper for KivoMusicQt validation scripts
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

$vsDevCmd = "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
if (-not (Test-Path -LiteralPath $vsDevCmd)) {
    throw "VsDevCmd.bat not found: $vsDevCmd"
}

$environmentOutput = cmd.exe /d /s /c "`"$vsDevCmd`" -arch=x64 >NUL && set"
foreach ($line in $environmentOutput) {
    $separator = $line.IndexOf("=")
    if ($separator -le 0) {
        continue
    }

    $name = $line.Substring(0, $separator)
    $value = $line.Substring($separator + 1)
    Set-Item -Path "Env:$name" -Value $value
}

$env:VSLANG = "1033"

$qtRoot = Join-Path $ProjectRoot ".qt\6.8.3\msvc2022_64"
$qtBin = Join-Path $qtRoot "bin"
if (-not (Test-Path -LiteralPath (Join-Path $qtBin "qt-cmake.bat"))) {
    throw "Qt 6.8.3 MSVC kit not found under $qtRoot"
}

$ninjaPath = "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
$env:Path = "$qtBin;$ninjaPath;$env:Path"
$env:CMAKE_PREFIX_PATH = $qtRoot

[PSCustomObject]@{
    ProjectRoot = $ProjectRoot
    QtRoot = $qtRoot
    QtBin = $qtBin
    NinjaPath = $ninjaPath
}
