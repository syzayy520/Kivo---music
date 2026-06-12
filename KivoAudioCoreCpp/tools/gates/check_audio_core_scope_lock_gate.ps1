# =============================================================================
# check_audio_core_scope_lock_gate.ps1
# Audio Core Scope Lock Gate - Video/Out-of-Scope Drift Detection
# =============================================================================
# Scans for video renderer, video surface, video HDR, and out-of-scope tokens.
# Prevents scope drift into video rendering domain.
# P0-014: OUT_OF_SCOPE_VIDEO_RENDERER_DISCARDED
# P0-013: NO_GO_ALLOWLIST_MISSING_NO_IMPLEMENTATION
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

# Resolve PROJECT_ROOT - always resolve to absolute path
if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== Audio Core Scope Lock Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

# =============================================================================
# Section 1: Forbidden Video Renderer Tokens
# =============================================================================
$videoRendererTokens = @(
    "Video Renderer",
    "RenderFrame",
    "RenderTarget",
    "RenderSubmission",
    "RenderOutcome"
)

# Scan repository-controlled source files only. Generated build, release,
# dependency checkout, and self-test output must never influence scope truth.
$excludedRoots = @(
    ".git",
    ".build",
    "build",
    "out",
    "node_modules"
) | ForEach-Object {
    ([System.IO.Path]::GetFullPath(
        (Join-Path $ProjectRoot $_))).TrimEnd("\", "/")
}

function Test-KivoScopeExcludedPath {
    param([Parameter(Mandatory = $true)][string]$FullPath)

    $candidate = [System.IO.Path]::GetFullPath($FullPath)
    foreach ($root in $excludedRoots) {
        if ($candidate -eq $root -or
            $candidate.StartsWith(
                $root + [System.IO.Path]::DirectorySeparatorChar,
                [System.StringComparison]::OrdinalIgnoreCase)) {
            return $true
        }
    }
    return $false
}

function Get-KivoScopeRelativePath {
    param([Parameter(Mandatory = $true)][string]$FullPath)

    return $FullPath.Substring($ProjectRoot.Length).
        TrimStart("\", "/").Replace("\", "/")
}

$sourceFiles = Get-ChildItem -Path $ProjectRoot -Recurse -File -Include "*.cpp","*.h","*.hpp","*.c","*.ps1","*.md","*.txt","*.json" -ErrorAction SilentlyContinue |
    Where-Object {
        -not (Test-KivoScopeExcludedPath -FullPath $_.FullName) -and
        $_.Name -ne "audio_core_scope_lock_policy.md" -and
        $_.Name -ne "check_audio_core_scope_lock_gate.ps1"
    }

foreach ($file in $sourceFiles) {
    $content = Get-Content $file.FullName -Raw -ErrorAction SilentlyContinue
    if ($content) {
        foreach ($token in $videoRendererTokens) {
            if ($content -match [regex]::Escape($token)) {
                $relativePath = Get-KivoScopeRelativePath `
                    -FullPath $file.FullName
                $violations += "VIDEO_RENDERER_SCOPE_DRIFT: $relativePath contains: $token"
            }
        }
    }
}

# =============================================================================
# Section 2: Forbidden Video Surface / Swapchain Tokens
# =============================================================================
$videoSurfaceTokens = @(
    "video surface",
    "swapchain",
    "framebuffer"
)

foreach ($file in $sourceFiles) {
    $content = Get-Content $file.FullName -Raw -ErrorAction SilentlyContinue
    if ($content) {
        foreach ($token in $videoSurfaceTokens) {
            # Case-insensitive search
            if ($content -match "(?i)" + [regex]::Escape($token)) {
                $relativePath = Get-KivoScopeRelativePath `
                    -FullPath $file.FullName
                # Skip docs/policy mentions (DOC_MENTION_ALLOWED)
                if ($relativePath -like "docs/*" -or $relativePath -like "tools/gates/*") {
                    continue
                }
                $violations += "VIDEO_SURFACE_SCOPE_DRIFT: $relativePath contains: $token"
            }
        }
    }
}

# =============================================================================
# Section 3: Forbidden Video HDR Tokens
# =============================================================================
$videoHdrTokens = @(
    "video HDR",
    "HDR pipeline",
    "HDR metadata"
)

foreach ($file in $sourceFiles) {
    $content = Get-Content $file.FullName -Raw -ErrorAction SilentlyContinue
    if ($content) {
        foreach ($token in $videoHdrTokens) {
            if ($content -match [regex]::Escape($token)) {
                $relativePath = Get-KivoScopeRelativePath `
                    -FullPath $file.FullName
                if ($relativePath -like "docs/*" -or $relativePath -like "tools/gates/*") {
                    continue
                }
                $violations += "VIDEO_HDR_SCOPE_DRIFT: $relativePath contains: $token"
            }
        }
    }
}

# =============================================================================
# Section 4: Forbidden Video Render Contract Paths
# =============================================================================
$forbiddenPaths = @(
    "src\render\contracts",
    "tests\render\contracts",
    "src\render",
    "tests\render"
)

foreach ($path in $forbiddenPaths) {
    $fullPath = Join-Path $ProjectRoot $path
    if (Test-Path $fullPath) {
        $violations += "VIDEO_RENDER_CONTRACTS_BLOCKER: Forbidden path exists: $path/"
    }
}

# =============================================================================
# Section 5: P0-013 Restricted Device Contract Paths
# =============================================================================
# These paths are RESTRICTED (not permanently forbidden).
# P0-013 is blocked until complete allowlist + dependency proof + audio-only confirmation.
$p013RestrictedPaths = @(
    "src\device\contracts",
    "tests\device\contracts"
)

foreach ($path in $p013RestrictedPaths) {
    $fullPath = Join-Path $ProjectRoot $path
    if (Test-Path $fullPath) {
        $violations += "P0_013_RESTRICTED: $path/ exists - P0-013 is blocked until complete allowlist + dependency proof + audio-only confirmation"
    }
}

# Restricted device tokens (audio device family, not video)
$p013RestrictedTokens = @(
    "DeviceId",
    "DeviceCapabilitySnapshot",
    "DeviceMemorySnapshot",
    "RecoveryTimelineImpact"
)

foreach ($file in $sourceFiles) {
    $content = Get-Content $file.FullName -Raw -ErrorAction SilentlyContinue
    if ($content) {
        foreach ($token in $p013RestrictedTokens) {
            if ($content -match [regex]::Escape($token)) {
                $relativePath = Get-KivoScopeRelativePath `
                    -FullPath $file.FullName
                # Skip docs/policy mentions
                if ($relativePath -like "docs/*" -or $relativePath -like "tools/gates/*") {
                    continue
                }
                $violations += "P0_013_RESTRICTED: $relativePath contains: $token - P0-013 is blocked until complete allowlist + dependency proof + audio-only confirmation"
            }
        }
    }
}

# =============================================================================
# Section 6: Allowed Audio Core Tokens (Verification)
# =============================================================================
# These tokens are legitimate audio core content and must NOT be flagged.
# This section is for documentation only - no scanning needed.

$allowedTokens = @(
    "kivo_capability_tests",
    "kivo_contract_tests",
    "include/kivo/core/contract/capability",
    "audio",
    "clock",
    "seek",
    "format",
    "source",
    "memory",
    "output",
    "RenderClientBoundary"  # Audio render client, NOT video render
)

# =============================================================================
# Report
# =============================================================================
Write-Host "--- Scan Scope ---"
Write-Host "Forbidden Video Renderer Tokens: Video Renderer, RenderFrame, RenderTarget, RenderSubmission, RenderOutcome"
Write-Host "Forbidden Video Surface Tokens: video surface, swapchain, framebuffer"
Write-Host "Forbidden Video HDR Tokens: video HDR, HDR pipeline, HDR metadata"
Write-Host "Forbidden Paths: src/render/contracts, tests/render/contracts, src/render, tests/render"
Write-Host "Restricted P0-013 Paths: src/device/contracts, tests/device/contracts"
Write-Host "Restricted P0-013 Tokens: DeviceId, DeviceCapabilitySnapshot, DeviceMemorySnapshot, RecoveryTimelineImpact"
Write-Host "Allowed Audio Tokens: kivo_capability_tests, kivo_contract_tests, audio, clock, seek, format, source, memory, output, RenderClientBoundary"
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "AUDIO_CORE_SCOPE_LOCK_PASSED"
    Write-Host "No video/out-of-scope drift detected. Audio core scope is locked."
    exit 0
} else {
    Write-Host "FAIL"
    Write-Host "SCOPE_DRIFT_DETECTED"
    Write-Host "Violations found ($($violations.Count)):"
    foreach ($v in $violations) {
        Write-Host "  - $v"
    }
    exit 1
}
