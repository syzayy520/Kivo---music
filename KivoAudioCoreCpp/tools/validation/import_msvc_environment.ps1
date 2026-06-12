# =============================================================================
# import_msvc_environment.ps1
# Imports the latest installed x64 MSVC developer environment into this process.
# Dot-source this file, then call Import-KivoMsvcEnvironment.
# =============================================================================

function Import-KivoMsvcEnvironment {
    [CmdletBinding()]
    param()

    function Test-MsvcEnvironmentReady {
        $cl = Get-Command cl.exe -ErrorAction SilentlyContinue
        $include = [Environment]::GetEnvironmentVariable("INCLUDE", "Process")
        $lib = [Environment]::GetEnvironmentVariable("LIB", "Process")

        return $null -ne $cl -and
            $include -and
            $include -match "VC\\Tools\\MSVC" -and
            $lib -and
            $lib -match "VC\\Tools\\MSVC"
    }

    # Set this before vcvarsall so every child process inherits stable
    # /showIncludes output that CMake and Ninja can recognize.
    [Environment]::SetEnvironmentVariable("VSLANG", "1033", "Process")

    if (Test-MsvcEnvironmentReady) {
        return $true
    }

    $vswhereCandidates = @(
        (Join-Path ([Environment]::GetEnvironmentVariable("ProgramFiles(x86)")) "Microsoft Visual Studio\Installer\vswhere.exe"),
        (Join-Path ([Environment]::GetEnvironmentVariable("ProgramFiles")) "Microsoft Visual Studio\Installer\vswhere.exe")
    ) | Where-Object { $_ -and (Test-Path $_) }

    $installationPath = $null
    $vswhere = $vswhereCandidates | Select-Object -First 1
    if ($vswhere) {
        $installationPath = & $vswhere -latest -products * `
            -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
            -property installationPath 2>$null
    }

    if (-not $installationPath) {
        $fallbacks = @(
            "C:\Program Files\Microsoft Visual Studio\18\Community",
            "C:\Program Files\Microsoft Visual Studio\2022\Community",
            "C:\Program Files\Microsoft Visual Studio\2022\BuildTools",
            "C:\Program Files\Microsoft Visual Studio\2022\Professional",
            "C:\Program Files\Microsoft Visual Studio\2022\Enterprise"
        )
        $installationPath = $fallbacks | Where-Object { Test-Path $_ } | Select-Object -First 1
    }

    if (-not $installationPath) {
        return $false
    }

    $vcvarsall = Join-Path $installationPath "VC\Auxiliary\Build\vcvarsall.bat"
    if (-not (Test-Path $vcvarsall)) {
        return $false
    }

    $environmentLines = cmd.exe /d /s /c "`"$vcvarsall`" x64 >nul 2>&1 && set"
    if ($LASTEXITCODE -ne 0) {
        return $false
    }

    foreach ($line in $environmentLines) {
        if ($line -match "^([^=]+)=(.*)$") {
            [Environment]::SetEnvironmentVariable($Matches[1], $Matches[2], "Process")
        }
    }

    return Test-MsvcEnvironmentReady
}
