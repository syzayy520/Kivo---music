# =============================================================================
# check_genealogy_gate.ps1
# Genealogy Gate — Directory Structure Validation
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== Genealogy Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$allowedTopLevel = @(
    "README.md",
    "CMakeLists.txt",
    "CMakePresets.json",
    ".editorconfig",
    ".gitignore",
    ".clang-format",
    "docs",
    "tools",
    "include",
    "src",
    "tests",
    ".build",
    "build"
)

$blockedTopLevelDirs = @(
    "platform",
    "decode",
    "playback",
    "dsp",
    "diagnostics",
    "examples",
    "demo",
    "sample_player",
    "third_party",
    "vendor",
    "bin",
    "lib"
)

$violations = @()

foreach ($dir in $blockedTopLevelDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $violations += "BLOCKED_TOP_LEVEL_DIRECTORY: $dir/ exists"
    }
}

$actualItems = Get-ChildItem -Path $ProjectRoot -Force | Where-Object { $_.Name -ne ".git" }
foreach ($item in $actualItems) {
    if ($item.Name -notin $allowedTopLevel) {
        $violations += "UNAUTHORIZED_ITEM: $($item.Name)"
    }
}

$docsPath = Join-Path $ProjectRoot "docs"
if (Test-Path $docsPath) {
    $docsItems = Get-ChildItem -Path $docsPath -Directory
    foreach ($item in $docsItems) {
        if ($item.Name -notin @(
            "architecture",
            "closeout",
            "dependencies",
            "tasks",
            "validation"
        )) {
            $violations += "UNAUTHORIZED_DOCS_DIR: docs/$($item.Name)/"
        }
    }
}

$toolsPath = Join-Path $ProjectRoot "tools"
if (Test-Path $toolsPath) {
    $toolsItems = Get-ChildItem -Path $toolsPath -Directory
    foreach ($item in $toolsItems) {
        if ($item.Name -notin @("gates", "validation")) {
            $violations += "UNAUTHORIZED_TOOLS_DIR: tools/$($item.Name)/"
        }
    }
}

$runtimeFamilyRules = @(
    @{
        Path = "tests\decode_boundary"
        AllowedDirectories = @(
            "fixture",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "tests\hardware"
        AllowedDirectories = @(
            "decode_output",
            "device_matrix",
            "wasapi"
        )
    },
    @{
        Path = "tests\hardware\device_matrix"
        AllowedDirectories = @(
            "fixture",
            "platform",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "tests\hardware\device_matrix\platform"
        AllowedDirectories = @("windows")
    },
    @{
        Path = "tests\hardware\device_matrix\platform\windows"
        AllowedDirectories = @(
            "apartment",
            "driver",
            "endpoint"
        )
    },
    @{
        Path = "tests\hardware\decode_output"
        AllowedDirectories = @(
            "fixture",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "tests\hardware\wasapi"
        AllowedDirectories = @(
            "fixture",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "tests\stability"
        AllowedDirectories = @(
            "fixture",
            "platform",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "tests\stability\platform"
        AllowedDirectories = @("windows")
    },
    @{
        Path = "include\kivo\adapters\ffmpeg"
        AllowedDirectories = @("session")
    },
    @{
        Path = "src\adapters\ffmpeg"
        AllowedDirectories = @(
            "codec",
            "container",
            "conversion",
            "mapping",
            "session",
            "source"
        )
    },
    @{
        Path = "tests\ffmpeg_decode"
        AllowedDirectories = @(
            "fixture",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "tests\ffmpeg_decode\fixture"
        AllowedDirectories = @(
            "assertion",
            "decode",
            "harness",
            "mutation"
        )
    },
    @{
        Path = "tests\ffmpeg_decode\scenario"
        AllowedDirectories = @(
            "control",
            "corruption",
            "cue",
            "decode",
            "policy",
            "truncation"
        )
    },
    @{
        Path = "include\kivo\platform\windows\wasapi"
        AllowedDirectories = @(
            "diagnostics",
            "renderer",
            "result",
            "worker"
        )
    },
    @{
        Path = "src\platform\windows\wasapi"
        AllowedDirectories = @(
            "apartment",
            "device",
            "error",
            "format",
            "power",
            "renderer",
            "worker"
        )
    },
    @{
        Path = "src\platform\windows\wasapi\device"
        AllowedDirectories = @("notification")
    },
    @{
        Path = "src\platform\windows\wasapi\power"
        AllowedDirectories = @("notification")
    },
    @{
        Path = "tests\platform_windows\wasapi"
        AllowedDirectories = @(
            "fixture",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "include\kivo\testing\render"
        AllowedDirectories = @(
            "configuration",
            "fault",
            "observation",
            "renderer"
        )
    },
    @{
        Path = "src\testing\render"
        AllowedDirectories = @(
            "consumption",
            "fault",
            "lifecycle",
            "observation",
            "write"
        )
    },
    @{
        Path = "tests\fake_renderer"
        AllowedDirectories = @(
            "fixture",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "include\kivo\core\playback\pipeline"
        AllowedDirectories = @(
            "configuration",
            "producer",
            "result",
            "snapshot"
        )
    },
    @{
        Path = "src\core\playback\pipeline"
        AllowedDirectories = @(
            "buffer",
            "producer",
            "state",
            "step",
            "transfer"
        )
    },
    @{
        Path = "tests\playback_pipeline"
        AllowedDirectories = @(
            "fixture",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "include\kivo\core\playback\gapless"
        AllowedDirectories = @(
            "coordinator",
            "plan",
            "position",
            "segment",
            "transition"
        )
    },
    @{
        Path = "src\core\playback\gapless"
        AllowedDirectories = @(
            "coordinator",
            "position",
            "segment",
            "transition"
        )
    },
    @{
        Path = "tests\playback_gapless"
        AllowedDirectories = @(
            "fixture",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "tests\playback_gapless\scenario"
        AllowedDirectories = @(
            "compatibility",
            "continuity",
            "position",
            "segment"
        )
    },
    @{
        Path = "include\kivo\core\playback\recovery"
        AllowedDirectories = @(
            "classification",
            "completion",
            "execution",
            "executor",
            "operations",
            "policy"
        )
    },
    @{
        Path = "src\core\playback\recovery"
        AllowedDirectories = @("executor")
    },
    @{
        Path = "include\kivo\core\playback\session"
        AllowedDirectories = @(
            "command",
            "controller",
            "drain",
            "snapshot"
        )
    },
    @{
        Path = "src\core\playback\session"
        AllowedDirectories = @(
            "command",
            "controller",
            "drain",
            "position",
            "recovery",
            "result",
            "seek",
            "snapshot",
            "state"
        )
    },
    @{
        Path = "tests\playback_session"
        AllowedDirectories = @(
            "fixture",
            "runner",
            "scenario"
        )
    },
    @{
        Path = "include\kivo\core\playback\runtime"
        AllowedDirectories = @(
            "coordinator",
            "failure",
            "request",
            "result",
            "snapshot"
        )
    },
    @{
        Path = "src\core\playback\runtime"
        AllowedDirectories = @(
            "command",
            "coordinator",
            "drain",
            "flush",
            "lifecycle",
            "open",
            "recovery",
            "resource",
            "result",
            "seek",
            "snapshot",
            "state",
            "step",
            "transport"
        )
    },
    @{
        Path = "tests\playback_runtime"
        AllowedDirectories = @("coordinator")
    },
    @{
        Path = "tests\playback_runtime\coordinator"
        AllowedDirectories = @(
            "fixture",
            "runner",
            "scenario"
        )
    }
)

foreach ($rule in $runtimeFamilyRules) {
    $familyPath = Join-Path $ProjectRoot $rule.Path
    if (-not (Test-Path $familyPath)) {
        continue
    }

    $rootFiles = Get-ChildItem -Path $familyPath -File -Force
    foreach ($file in $rootFiles) {
        $violations += "FLAT_FAMILY_FILE: $($rule.Path)\$($file.Name)"
    }

    $childDirectories = Get-ChildItem -Path $familyPath -Directory -Force
    foreach ($directory in $childDirectories) {
        if ($directory.Name -notin $rule.AllowedDirectories) {
            $violations += "UNAUTHORIZED_RUNTIME_SUBFAMILY: $($rule.Path)\$($directory.Name)\"
        }
    }
}

$runtimeSourceRoots = @(
    "tests\decode_boundary",
    "tests\hardware",
    "tests\stability",
    "include\kivo\adapters\ffmpeg",
    "src\adapters\ffmpeg",
    "tests\ffmpeg_decode",
    "include\kivo\platform\windows\wasapi",
    "src\platform\windows\wasapi",
    "tests\platform_windows\wasapi",
    "include\kivo\testing\render",
    "src\testing\render",
    "tests\fake_renderer",
    "include\kivo\core\playback\pipeline",
    "include\kivo\core\playback\recovery",
    "include\kivo\core\playback\session",
    "include\kivo\core\playback\runtime",
    "src\core\playback\pipeline",
    "include\kivo\core\playback\gapless",
    "src\core\playback\gapless",
    "src\core\playback\recovery",
    "src\core\playback\session",
    "src\core\playback\runtime",
    "tests\playback_pipeline",
    "tests\playback_gapless",
    "tests\playback_session",
    "tests\playback_runtime"
)
$runtimeMaximumLines = 260
foreach ($relativeRoot in $runtimeSourceRoots) {
    $runtimeRoot = Join-Path $ProjectRoot $relativeRoot
    if (-not (Test-Path $runtimeRoot)) {
        continue
    }
    $runtimeFiles = Get-ChildItem -Path $runtimeRoot -Recurse -File |
        Where-Object { $_.Extension -in @(".hpp", ".cpp") }
    foreach ($file in $runtimeFiles) {
        $lineCount = (Get-Content $file.FullName).Count
        if ($lineCount -gt $runtimeMaximumLines) {
            $relativeFile = $file.FullName.Substring($ProjectRoot.Length).TrimStart("\")
            $violations += "RUNTIME_FILE_RESPONSIBILITY_OVERFLOW: $relativeFile has $lineCount lines (max $runtimeMaximumLines)"
        }
    }
}

Write-Host "--- Allowed Top-Level Items ---"
foreach ($a in $allowedTopLevel) {
    Write-Host "  $a"
}
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "Directory structure follows current contract genealogy."
    exit 0
}

Write-Host "FAIL"
Write-Host "Violations found ($($violations.Count)):"
foreach ($v in $violations) {
    Write-Host "  - $v"
}
exit 1
