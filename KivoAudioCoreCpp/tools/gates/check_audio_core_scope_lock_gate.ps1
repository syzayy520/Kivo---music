# =============================================================================
# check_audio_core_scope_lock_gate.ps1
# Audio Core Scope Lock Gate — Video/Out-of-Scope Drift Detection
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

# Resolve PROJECT_ROOT
if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
    $ProjectRoot = Resolve-Path $ProjectRoot
}

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

# Scan all source files
# Exclude: .git, .build, node_modules, scope lock policy doc, this gate script
$sourceFiles = Get-ChildItem -Path $ProjectRoot -Recurse -File -Include "*.cpp","*.h","*.hpp","*.c","*.ps1","*.md","*.txt","*.json" -ErrorAction SilentlyContinue |
    Where-Object {
        $normPath = $_.FullName.ToLower()
        $normPath -notlike "*\.git\*" -and
        $normPath -notlike "*\.build\*" -and
        $normPath -notlike "*\node_modules\*" -and
        $_.Name -ne "audio_core_scope_lock_policy.md" -and
        $_.Name -ne "check_audio_core_scope_lock_gate.ps1"
    }

foreach ($file in $sourceFiles) {
    $content = Get-Content $file.FullName -Raw -ErrorAction SilentlyContinue
    if ($content) {
        foreach ($token in $videoRendererTokens) {
            if ($content -match [regex]::Escape($token)) {
                $relativePath = $file.FullName.Replace($ProjectRoot, "").TrimStart("\", "/")
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
                $relativePath = $file.FullName.Replace($ProjectRoot, "").TrimStart("\", "/")
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
                $relativePath = $file.FullName.Replace($ProjectRoot, "").TrimStart("\", "/")
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
# Section 5: P0-013 Unauthorized Device Contract Paths
# =============================================================================
$p013ForbiddenPaths = @(
    "src\device\contracts",
    "tests\device\contracts",
    "src\device",
    "tests\device"
)

foreach ($path in $p013ForbiddenPaths) {
    $fullPath = Join-Path $ProjectRoot $path
    if (Test-Path $fullPath) {
        $violations += "P0_013_UNAUTHORIZED_BLOCKER: P0-013 path exists without allowlist: $path/"
    }
}

# =============================================================================
# Section 6: Allowed Audio Core Tokens (Verification)
# =============================================================================
# These tokens are legitimate audio core content and must NOT be flagged.
# This section is for documentation only — no scanning needed.

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
Write-Host "Forbidden P0-013 Paths: src/device/contracts, tests/device/contracts, src/device, tests/device"
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
