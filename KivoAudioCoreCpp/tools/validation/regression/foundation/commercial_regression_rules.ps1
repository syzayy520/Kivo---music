Set-StrictMode -Version Latest

function Test-KivoRegressionTextPrivacy {
    param([AllowNull()][string]$Value)

    if (-not $Value) {
        return $true
    }
    $forbidden = @(
        "[A-Za-z]:\\Users\\",
        "(?i)authorization\s*:",
        "(?i)bearer\s+[A-Za-z0-9._-]+",
        "(?i)(password|passwd|token|secret)\s*=",
        "(?i)https?://[^\s/@]+:[^\s/@]+@"
    )
    foreach ($pattern in $forbidden) {
        if ($Value -match $pattern) {
            return $false
        }
    }
    return $true
}

function Test-KivoNumericDiagnosticObject {
    param($Value)

    if ($null -eq $Value) {
        return $true
    }
    if ($Value -is [string]) {
        return $false
    }
    if ($Value -is [bool] -or
        $Value -is [byte] -or
        $Value -is [int16] -or
        $Value -is [int32] -or
        $Value -is [int64] -or
        $Value -is [uint16] -or
        $Value -is [uint32] -or
        $Value -is [uint64] -or
        $Value -is [single] -or
        $Value -is [double] -or
        $Value -is [decimal]) {
        return $true
    }
    if ($Value -is [System.Collections.IEnumerable] -and
        $Value -isnot [System.Collections.IDictionary] -and
        $Value -isnot [PSCustomObject]) {
        foreach ($item in $Value) {
            if (-not (Test-KivoNumericDiagnosticObject -Value $item)) {
                return $false
            }
        }
        return $true
    }
    foreach ($property in $Value.PSObject.Properties) {
        if (-not (Test-KivoNumericDiagnosticObject `
                -Value $property.Value)) {
            return $false
        }
    }
    return $true
}

function Test-KivoGitCommit {
    param([AllowNull()][string]$Value)
    return $Value -and $Value -match "^[0-9a-fA-F]{40}$"
}
