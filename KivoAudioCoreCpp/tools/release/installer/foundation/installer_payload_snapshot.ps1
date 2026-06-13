Set-StrictMode -Version Latest

function Get-KivoTextSha256 {
    param([Parameter(Mandatory = $true)][string]$Value)

    $algorithm = [System.Security.Cryptography.SHA256]::Create()
    try {
        $bytes = [System.Text.Encoding]::UTF8.GetBytes($Value)
        return (
            [System.BitConverter]::ToString(
                $algorithm.ComputeHash($bytes))).Replace("-", "").ToLowerInvariant()
    } finally {
        $algorithm.Dispose()
    }
}

function Test-KivoRelativePayloadPath {
    param([Parameter(Mandatory = $true)][string]$Path)

    $normalized = $Path.Replace("\", "/")
    return $normalized -and
        -not [System.IO.Path]::IsPathRooted($normalized) -and
        -not $normalized.StartsWith("../") -and
        -not $normalized.Contains("/../") -and
        -not $normalized.EndsWith("/..")
}

function Get-KivoInstallerRelativePath {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [Parameter(Mandatory = $true)][string]$Path
    )

    $rootUri = [Uri](
        [System.IO.Path]::GetFullPath($Root).TrimEnd("\", "/") + "\")
    $pathUri = [Uri][System.IO.Path]::GetFullPath($Path)
    return [Uri]::UnescapeDataString(
        $rootUri.MakeRelativeUri($pathUri).ToString())
}

function Read-KivoInstallerJson {
    param([Parameter(Mandatory = $true)][string]$Path)

    $resolved = (Resolve-Path -LiteralPath $Path).Path
    return [System.IO.File]::ReadAllText(
        $resolved,
        [System.Text.UTF8Encoding]::new($false)) |
        ConvertFrom-Json
}

function Get-KivoInstallerPayloadSnapshot {
    param([Parameter(Mandatory = $true)][string]$RuntimeDirectory)

    $runtime = (Resolve-Path -LiteralPath $RuntimeDirectory).Path
    $manifestPath = Join-Path $runtime "manifest\build-manifest.json"
    $errors = [System.Collections.Generic.List[string]]::new()
    if (-not (Test-Path -LiteralPath $manifestPath)) {
        throw "Build manifest is missing: $manifestPath"
    }

    $manifest = Read-KivoInstallerJson -Path $manifestPath
    if ($manifest.schema -ne "kivo.build-manifest.v1") {
        $errors.Add("Unsupported build manifest schema: $($manifest.schema)")
    }

    $expected = @{}
    $fileRecords = [System.Collections.Generic.List[object]]::new()
    foreach ($file in @($manifest.files)) {
        $path = [string]$file.path
        if (-not (Test-KivoRelativePayloadPath -Path $path)) {
            $errors.Add("Unsafe manifest path: $path")
            continue
        }
        $key = $path.Replace("\", "/").ToLowerInvariant()
        if ($expected.ContainsKey($key)) {
            $errors.Add("Duplicate manifest path: $path")
            continue
        }
        $expected[$key] = $file
        $absolute = Join-Path $runtime ($path.Replace("/", "\"))
        if (-not (Test-Path -LiteralPath $absolute -PathType Leaf)) {
            $errors.Add("Missing payload file: $path")
            continue
        }
        $item = Get-Item -LiteralPath $absolute
        $actualHash = (Get-FileHash -LiteralPath $absolute `
            -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($item.Length -ne [int64]$file.size) {
            $errors.Add("Size mismatch: $path")
        }
        if ($actualHash -ne ([string]$file.sha256).ToLowerInvariant()) {
            $errors.Add("Hash mismatch: $path")
        }
        $fileRecords.Add([ordered]@{
            path = $path.Replace("\", "/")
            size = $item.Length
            sha256 = $actualHash
        })
    }

    $actualPaths = Get-ChildItem -LiteralPath $runtime -Recurse -File |
        Where-Object {
            $_.FullName -ne $manifestPath
        } |
        ForEach-Object {
            $relative = Get-KivoInstallerRelativePath `
                -Root $runtime `
                -Path $_.FullName
            $relative.ToLowerInvariant()
        }
    foreach ($actualPath in $actualPaths) {
        if (-not $expected.ContainsKey($actualPath)) {
            $errors.Add("Unexpected payload file: $actualPath")
        }
    }

    $installerPath = Join-Path $runtime "manifest\installer-payload.json"
    if (-not (Test-Path -LiteralPath $installerPath)) {
        $errors.Add("Installer payload contract is missing.")
    } else {
        $installer = Read-KivoInstallerJson -Path $installerPath
        if ($installer.schema -ne "kivo.installer-payload.v1") {
            $errors.Add(
                "Unsupported installer payload schema: $($installer.schema)")
        }
        foreach ($required in @($installer.required_files)) {
            $key = ([string]$required).Replace("\", "/").ToLowerInvariant()
            if (-not $expected.ContainsKey($key)) {
                $errors.Add(
                    "Required installer file is absent from manifest: $required")
            }
        }
    }

    $corePath = Join-Path $runtime "bin\kivo_audio_core.dll"
    $coreSignature = if (Test-Path -LiteralPath $corePath) {
        (Get-AuthenticodeSignature -LiteralPath $corePath).Status.ToString()
    } else {
        "Missing"
    }
    $fingerprintInput = @(
        $fileRecords |
            Sort-Object { $_.path } |
            ForEach-Object {
                "$($_.path)|$($_.size)|$($_.sha256)"
            }
    ) -join "`n"

    return [PSCustomObject]@{
        RuntimeDirectory = $runtime
        Valid = $errors.Count -eq 0
        Errors = @($errors)
        Version = [string]$manifest.version
        SourceCommit = [string]$manifest.source.commit
        ManifestSigned = [bool]$manifest.signing.signed
        CoreSignatureStatus = $coreSignature
        Fingerprint = Get-KivoTextSha256 -Value $fingerprintInput
        Files = @($fileRecords)
    }
}

function Compare-KivoPayloadSnapshot {
    param(
        [Parameter(Mandatory = $true)]$Expected,
        [Parameter(Mandatory = $true)]$Actual,
        [Parameter(Mandatory = $true)][string]$Label
    )

    $errors = [System.Collections.Generic.List[string]]::new()
    if (-not $Expected.Valid) {
        foreach ($error in $Expected.Errors) {
            $errors.Add("${Label} expected payload: $error")
        }
    }
    if (-not $Actual.Valid) {
        foreach ($error in $Actual.Errors) {
            $errors.Add("${Label} actual payload: $error")
        }
    }
    if ($Expected.Fingerprint -ne $Actual.Fingerprint) {
        $errors.Add(
            "$Label payload fingerprint does not match the expected full set.")
    }
    if ($Expected.Version -ne $Actual.Version) {
        $errors.Add("$Label payload version mismatch.")
    }
    if ($Expected.SourceCommit -ne $Actual.SourceCommit) {
        $errors.Add("$Label source commit mismatch.")
    }
    return @($errors)
}
