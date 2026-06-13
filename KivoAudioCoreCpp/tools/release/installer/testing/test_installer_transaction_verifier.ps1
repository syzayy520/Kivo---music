param([string]$ProjectRoot)

$ErrorActionPreference = "Stop"
if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\..\..\.."
}
$ProjectRoot = (Resolve-Path -LiteralPath $ProjectRoot).Path
. (Join-Path $ProjectRoot `
    "tools\release\foundation\release_foundation.ps1")

$testRoot = Join-Path $ProjectRoot "out\installer-transaction-selftest"
$testRoot = Reset-KivoDirectory `
    -Path $testRoot `
    -AllowedParent (Join-Path $ProjectRoot "out")
$verifier = Join-Path $ProjectRoot `
    "tools\release\installer\verification\verify_installer_transaction.ps1"

function New-TestPayload {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][string]$Version,
        [Parameter(Mandatory = $true)][string]$Commit,
        [Parameter(Mandatory = $true)][string]$Content
    )

    $bin = Join-Path $Path "bin"
    $manifestDirectory = Join-Path $Path "manifest"
    New-Item -ItemType Directory `
        -Path $bin,$manifestDirectory `
        -Force | Out-Null
    $core = Join-Path $bin "kivo_audio_core.dll"
    [System.IO.File]::WriteAllText(
        $core,
        $Content,
        [System.Text.UTF8Encoding]::new($false))
    $installerPath = Join-Path $manifestDirectory "installer-payload.json"
    Write-KivoJson -Path $installerPath -Value ([ordered]@{
        schema = "kivo.installer-payload.v1"
        version = $Version
        required_files = @("bin/kivo_audio_core.dll")
        rules = @("Test fixture.")
    })
    $files = @(
        $core,
        $installerPath
    ) | ForEach-Object {
        $item = Get-Item -LiteralPath $_
        [ordered]@{
            path = Get-KivoRelativePath -Root $Path -Path $_
            size = $item.Length
            sha256 = Get-KivoSha256 -Path $_
        }
    }
    Write-KivoJson `
        -Path (Join-Path $manifestDirectory "build-manifest.json") `
        -Value ([ordered]@{
            schema = "kivo.build-manifest.v1"
            version = $Version
            source = [ordered]@{ commit = $Commit }
            signing = [ordered]@{ signed = $false }
            files = $files
        })
}

function Copy-TestPayload {
    param(
        [Parameter(Mandatory = $true)][string]$Source,
        [Parameter(Mandatory = $true)][string]$Destination
    )
    New-Item -ItemType Directory -Path $Destination -Force | Out-Null
    Copy-Item -Path (Join-Path $Source "*") `
        -Destination $Destination `
        -Recurse
}

function Invoke-ExpectedPass {
    param(
        [Parameter(Mandatory = $true)][string]$Mode,
        [Parameter(Mandatory = $true)][string]$Candidate,
        [Parameter(Mandatory = $true)][string]$Installed,
        [string]$Previous
    )
    $arguments = @(
        "-ExecutionPolicy", "Bypass",
        "-File", $verifier,
        "-Mode", $Mode,
        "-CandidateRuntimeDirectory", $Candidate,
        "-InstalledRuntimeDirectory", $Installed,
        "-ReportPath", (Join-Path $testRoot "$Mode-report.json")
    )
    if ($Previous) {
        $arguments += @("-PreviousRuntimeDirectory", $Previous)
    }
    & powershell @arguments | Out-Host
    if ($LASTEXITCODE -ne 0) {
        throw "$Mode self-test unexpectedly failed."
    }
}

$previous = Join-Path $testRoot "previous"
$candidate = Join-Path $testRoot "candidate"
$installed = Join-Path $testRoot "installed"
New-TestPayload -Path $previous -Version "1.0.0" -Commit ("a" * 40) `
    -Content "previous"
New-TestPayload -Path $candidate -Version "1.1.0" -Commit ("b" * 40) `
    -Content "candidate"

Copy-TestPayload -Source $candidate -Destination $installed
Invoke-ExpectedPass -Mode Clean -Candidate $candidate -Installed $installed
Invoke-ExpectedPass -Mode Upgrade -Candidate $candidate `
    -Installed $installed -Previous $previous

Remove-Item -LiteralPath $installed -Recurse -Force
Copy-TestPayload -Source $previous -Destination $installed
Invoke-ExpectedPass -Mode Rollback -Candidate $candidate `
    -Installed $installed -Previous $previous

[System.IO.File]::AppendAllText(
    (Join-Path $installed "bin\kivo_audio_core.dll"),
    "tampered")
$savedErrorActionPreference = $ErrorActionPreference
$ErrorActionPreference = "Continue"
$tamperOutput = & powershell -ExecutionPolicy Bypass -File $verifier `
    -Mode Rollback `
    -CandidateRuntimeDirectory $candidate `
    -InstalledRuntimeDirectory $installed `
    -PreviousRuntimeDirectory $previous `
    -ReportPath (Join-Path $testRoot "tamper-report.json") 2>&1
$tamperExitCode = $LASTEXITCODE
$ErrorActionPreference = $savedErrorActionPreference
if ($tamperExitCode -eq 0) {
    throw "Tampered payload was not rejected."
}

Write-Host "INSTALLER_TRANSACTION_SELFTEST_PASS"
exit 0
