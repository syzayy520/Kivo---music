# =============================================================================
# check_qt_family_gate.ps1
# KivoMusicQt family structure gate
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== KivoMusicQt Family Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

$required = @(
    "docs\architecture\family_gate.md",
    "docs\architecture\kivo_desktop_commercial_architecture.md",
    "ui\qml\tokens\Theme.qml",
    "ui\qml\windows\HomeWindow.qml",
    "ui\qml\components\navigation\Sidebar.qml",
    "ui\qml\components\transport\TransportBar.qml",
    "ui\qml\components\truth\TruthBadge.qml"
)

foreach ($relative in $required) {
    if (-not (Test-Path -LiteralPath (Join-Path $ProjectRoot $relative))) {
        $violations += "MISSING_REQUIRED_FAMILY_FILE: $relative"
    }
}

$directComponentFiles = Get-ChildItem `
    -Path (Join-Path $ProjectRoot "ui\qml\components") `
    -File `
    -ErrorAction SilentlyContinue
foreach ($file in $directComponentFiles) {
    $violations += "FLAT_COMPONENT_FILE: ui/qml/components/$($file.Name)"
}

$qmlFiles = Get-ChildItem -Path (Join-Path $ProjectRoot "ui\qml") `
    -Recurse -File -Filter "*.qml" -ErrorAction SilentlyContinue
foreach ($file in $qmlFiles) {
    $lineCount = (Get-Content -LiteralPath $file.FullName).Count
    $relativePath = $file.FullName.Substring($ProjectRoot.Length).
        TrimStart("\", "/").Replace("\", "/")
    $limit = if ($relativePath -like "ui/qml/windows/*") { 260 } else { 180 }
    if ($lineCount -gt $limit) {
        $violations += "QML_FILE_TOO_LARGE: $relativePath has $lineCount lines, limit $limit"
    }
}

$forbiddenPatterns = @(
    "KivoAudioCoreCpp",
    "sqlite3_",
    "QSqlDatabase",
    "LoadLibrary",
    "GetProcAddress"
)

foreach ($file in $qmlFiles) {
    $content = Get-Content -LiteralPath $file.FullName -Raw
    $relativePath = $file.FullName.Substring($ProjectRoot.Length).
        TrimStart("\", "/").Replace("\", "/")
    foreach ($pattern in $forbiddenPatterns) {
        if ($content -match [regex]::Escape($pattern)) {
            $violations += "QML_FORBIDDEN_NATIVE_OR_STORAGE_REFERENCE: $relativePath contains $pattern"
        }
    }
}

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "KIVO_QT_FAMILY_GATE_PASS"
    exit 0
}

Write-Host "FAIL"
Write-Host "KIVO_QT_FAMILY_GATE_FAIL"
Write-Host "Violations found ($($violations.Count)):"
foreach ($violation in $violations) {
    Write-Host "  - $violation"
}
exit 1
