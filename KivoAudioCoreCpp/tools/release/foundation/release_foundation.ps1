Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-KivoProjectRoot {
    $root = Join-Path $PSScriptRoot "..\..\.."
    return (Resolve-Path -LiteralPath $root).Path
}

function Assert-KivoPathInside {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][string]$Parent
    )
    $fullPath = [System.IO.Path]::GetFullPath($Path)
    $fullParent = [System.IO.Path]::GetFullPath($Parent).TrimEnd("\", "/")
    $prefix = $fullParent + [System.IO.Path]::DirectorySeparatorChar
    if (-not $fullPath.StartsWith(
        $prefix,
        [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Path must remain inside ${fullParent}: $fullPath"
    }
    return $fullPath
}

function Reset-KivoDirectory {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][string]$AllowedParent
    )
    $safePath = Assert-KivoPathInside -Path $Path -Parent $AllowedParent
    if (Test-Path -LiteralPath $safePath) {
        Remove-Item -LiteralPath $safePath -Recurse -Force
    }
    New-Item -ItemType Directory -Path $safePath -Force | Out-Null
    return $safePath
}

function Write-KivoJson {
    param(
        [Parameter(Mandatory = $true)]$Value,
        [Parameter(Mandatory = $true)][string]$Path
    )
    $parent = Split-Path -Parent $Path
    New-Item -ItemType Directory -Path $parent -Force | Out-Null
    $json = $Value | ConvertTo-Json -Depth 32
    [System.IO.File]::WriteAllText(
        $Path,
        $json + [Environment]::NewLine,
        [System.Text.UTF8Encoding]::new($false))
}

function Write-KivoText {
    param(
        [Parameter(Mandatory = $true)][string]$Value,
        [Parameter(Mandatory = $true)][string]$Path
    )
    $parent = Split-Path -Parent $Path
    New-Item -ItemType Directory -Path $parent -Force | Out-Null
    [System.IO.File]::WriteAllText(
        $Path,
        $Value,
        [System.Text.UTF8Encoding]::new($false))
}

function Get-KivoSha256 {
    param([Parameter(Mandatory = $true)][string]$Path)
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

function Get-KivoRelativePath {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [Parameter(Mandatory = $true)][string]$Path
    )
    $rootUri = [Uri](([System.IO.Path]::GetFullPath($Root).TrimEnd("\", "/")) + "\")
    $pathUri = [Uri][System.IO.Path]::GetFullPath($Path)
    return [Uri]::UnescapeDataString(
        $rootUri.MakeRelativeUri($pathUri).ToString())
}

function Get-KivoFfmpegRoot {
    param(
        [string]$RequestedRoot,
        [Parameter(Mandatory = $true)][string]$ProjectRoot
    )
    $candidates = @(
        $RequestedRoot,
        [Environment]::GetEnvironmentVariable("KIVO_FFMPEG_ROOT", "Process")
    )
    $cache = Join-Path $ProjectRoot "build\CMakeCache.txt"
    if (Test-Path -LiteralPath $cache) {
        $line = Select-String -Path $cache -Pattern "^KIVO_FFMPEG_ROOT:PATH=" |
            Select-Object -First 1
        if ($line) {
            $candidates += ($line.Line -split "=", 2)[1]
        }
    }
    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path -LiteralPath $candidate)) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }
    throw "KIVO_FFMPEG_ROOT could not be resolved."
}

function Assert-KivoCleanGit {
    param([Parameter(Mandatory = $true)][string]$ProjectRoot)
    Push-Location $ProjectRoot
    try {
        $status = git status --short
        if ($LASTEXITCODE -ne 0 -or $status) {
            throw "Release builds require a clean git worktree."
        }
        return [ordered]@{
            commit = (git rev-parse HEAD).Trim()
            branch = (git branch --show-current).Trim()
            commit_time = (git log -1 --format=%cI).Trim()
        }
    } finally {
        Pop-Location
    }
}

function New-KivoDeterministicZip {
    param(
        [Parameter(Mandatory = $true)][string]$SourceDirectory,
        [Parameter(Mandatory = $true)][string]$ArchivePath
    )
    Add-Type -AssemblyName System.IO.Compression
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    if (Test-Path -LiteralPath $ArchivePath) {
        Remove-Item -LiteralPath $ArchivePath -Force
    }
    $stream = [System.IO.File]::Open(
        $ArchivePath,
        [System.IO.FileMode]::CreateNew)
    try {
        $archive = [System.IO.Compression.ZipArchive]::new(
            $stream,
            [System.IO.Compression.ZipArchiveMode]::Create,
            $false)
        try {
            $files = Get-ChildItem -LiteralPath $SourceDirectory -Recurse -File |
                Sort-Object FullName
            foreach ($file in $files) {
                $relative = Get-KivoRelativePath `
                    -Root $SourceDirectory `
                    -Path $file.FullName
                $entry = $archive.CreateEntry(
                    $relative,
                    [System.IO.Compression.CompressionLevel]::Optimal)
                $entry.LastWriteTime = [DateTimeOffset]::new(
                    1980, 1, 1, 0, 0, 0, [TimeSpan]::Zero)
                $input = $file.OpenRead()
                try {
                    $output = $entry.Open()
                    try {
                        $input.CopyTo($output)
                    } finally {
                        $output.Dispose()
                    }
                } finally {
                    $input.Dispose()
                }
            }
        } finally {
            $archive.Dispose()
        }
    } finally {
        $stream.Dispose()
    }
}
