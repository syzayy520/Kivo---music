Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-KivoSanitizedRemote {
    param([Parameter(Mandatory = $true)][string]$Remote)

    $uri = $null
    if ([Uri]::TryCreate($Remote, [UriKind]::Absolute, [ref]$uri) -and
        $uri.Scheme -in @("http", "https", "ssh")) {
        $builder = [UriBuilder]::new($uri)
        $builder.UserName = ""
        $builder.Password = ""
        return $builder.Uri.AbsoluteUri
    }
    return $Remote
}

function Get-KivoCleanGitCheckout {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [Parameter(Mandatory = $true)][string]$Label
    )

    $resolved = (Resolve-Path -LiteralPath $Root).Path
    $commit = (& git -C $resolved rev-parse HEAD).Trim()
    if ($LASTEXITCODE -ne 0 -or $commit -notmatch "^[0-9a-fA-F]{40}$") {
        throw "$Label is not a readable Git checkout: $resolved"
    }
    $status = & git -C $resolved status --porcelain
    if ($LASTEXITCODE -ne 0 -or $status) {
        throw "$Label checkout must be clean: $resolved"
    }
    $remote = (& git -C $resolved remote get-url origin).Trim()
    if ($LASTEXITCODE -ne 0 -or -not $remote) {
        throw "$Label checkout must have an origin remote."
    }
    return [ordered]@{
        root = $resolved
        commit = $commit.ToLowerInvariant()
        remote = Get-KivoSanitizedRemote -Remote $remote
    }
}

function Assert-KivoRevisionMatch {
    param(
        [Parameter(Mandatory = $true)][string]$Actual,
        [Parameter(Mandatory = $true)][string]$Expected
    )

    if ($Expected -notmatch "^[0-9a-fA-F]{10,40}$") {
        throw "Expected revision must contain 10 to 40 hexadecimal characters."
    }
    if (-not $Actual.StartsWith(
            $Expected,
            [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Revision mismatch. Expected $Expected, found $Actual."
    }
}

function Get-KivoSourceInventory {
    param(
        [Parameter(Mandatory = $true)][string]$PayloadRoot,
        [Parameter(Mandatory = $true)][string[]]$RelativeRoots
    )

    $result = [System.Collections.Generic.List[object]]::new()
    foreach ($relativeRoot in $RelativeRoots) {
        $root = Join-Path $PayloadRoot $relativeRoot
        if (-not (Test-Path -LiteralPath $root -PathType Container)) {
            throw "Inventory root is missing: $relativeRoot"
        }
        Get-ChildItem -LiteralPath $root -Recurse -File |
            Where-Object { $_.FullName -notmatch "[\\/]\.git[\\/]" } |
            ForEach-Object {
                $relative = Get-KivoRelativePath `
                    -Root $PayloadRoot `
                    -Path $_.FullName
                $result.Add([ordered]@{
                    path = $relative.Replace("\", "/")
                    size = $_.Length
                    sha256 = Get-KivoSha256 -Path $_.FullName
                })
            }
    }
    $result.Sort([System.Comparison[object]]{
        param($left, $right)
        return [System.StringComparer]::Ordinal.Compare(
            [string]$left.path,
            [string]$right.path)
    })
    return $result.ToArray()
}

function Get-KivoFfmpegLicenses {
    param([Parameter(Mandatory = $true)][string]$SourceRoot)

    $licenses = @(
        @(
            "COPYING.LGPLv3",
            "COPYING.LGPLv2.1",
            "LICENSE.md"
        ) | Where-Object {
            Test-Path -LiteralPath (Join-Path $SourceRoot $_) -PathType Leaf
        }
    )
    if ($licenses.Count -eq 0) {
        throw "FFmpeg source does not contain an expected license file."
    }
    return @($licenses)
}

function Expand-KivoSafeSourceArchive {
    param(
        [Parameter(Mandatory = $true)][string]$ArchivePath,
        [Parameter(Mandatory = $true)][string]$Destination
    )

    Add-Type -AssemblyName System.IO.Compression
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    New-Item -ItemType Directory -Path $Destination -Force | Out-Null
    $destinationRoot = [System.IO.Path]::GetFullPath($Destination)
    $seen = [System.Collections.Generic.HashSet[string]]::new(
        [System.StringComparer]::OrdinalIgnoreCase)
    $archive = [System.IO.Compression.ZipFile]::OpenRead($ArchivePath)
    try {
        foreach ($entry in $archive.Entries) {
            $name = $entry.FullName.Replace("\", "/")
            if (-not $name -or $name.StartsWith("/") -or
                $name -match "^[A-Za-z]:" -or
                $name.Split("/") -contains ".." -or
                -not $seen.Add($name)) {
                throw "Unsafe or duplicate archive entry: $name"
            }
            if (-not $entry.Name) {
                continue
            }
            $target = Assert-KivoPathInside `
                -Path (Join-Path $destinationRoot $name) `
                -Parent $destinationRoot
            New-Item -ItemType Directory `
                -Path (Split-Path -Parent $target) -Force | Out-Null
            $entryStream = $entry.Open()
            try {
                $output = [System.IO.File]::Create($target)
                try {
                    $entryStream.CopyTo($output)
                } finally {
                    $output.Dispose()
                }
            } finally {
                $entryStream.Dispose()
            }
        }
    } finally {
        $archive.Dispose()
    }
}
