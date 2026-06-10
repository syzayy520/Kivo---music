# No Empty Forest Policy

**Purpose:** Prevent creation of empty directory forests and empty policy forests that create illusion of progress.  
**Scope:** All phases from P0-B onward.  
**Current Phase Rule:** P0-B creates only essential files with real content.  
**Last Updated:** P0-B (V10.1)  

## 1. Purpose

This policy prevents two common anti-patterns:
1. **Empty directory forest:** Creating directories (src/, core/, platform/, etc.) without implementation files
2. **Empty policy forest:** Creating policy files with only titles, TODOs, or placeholder text

## 2. Scope

Applies to all directory creation and policy file creation within KivoAudioCoreCpp/.

## 3. Current Phase Rule (P0-B)

P0-B creates only:
- Root skeleton files (README, CMake, presets, editorconfig, gitignore, clang-format)
- docs/architecture/ with substantive policy documents
- docs/closeout/ with non-empty closeout
- tools/gates/ with functional gate scripts

P0-B does NOT create: src/, core/, platform/, decode/, playback/, tests/, examples/, demo/, sample_player/

## 4. Allowed

- Creating directories only when they contain real files
- Creating policy files with Purpose/Scope/Rules/Allowed/Forbidden/Gate/Failure/Deferred
- Creating skeleton files with configure-only content (not placeholder)
- Creating gate scripts with functional logic

## 5. Forbidden

- Creating src/, core/, platform/, decode/, playback/, dsp/, diagnostics/, tests/ without P0-C authorization
- Creating third_party/, vendor/, bin/, lib/ without P0-G+ authorization
- Creating examples/, demo/, sample_player/ without P0-H+ authorization
- Policy files with only title and TODO
- Policy files with only placeholder text
- Policy files with only slogans and no actionable content
- Empty directories with no files
- Helper/utils/common/misc/glue/facade/types/defs bucket directories

## 6. Gate / Check Method

- `check_no_empty_forest_gate.ps1` scans for empty directories and empty policy files
- Checks directory has at least one non-hidden file
- Checks policy file has required sections with non-trivial content

## 7. Failure Classification

- `EMPTY_DIRECTORY_FOREST_BLOCKER`: Directory exists with no files
- `EMPTY_POLICY_FILE_BLOCKER`: Policy file exists with only placeholder
- `BUCKET_DIRECTORY_BLOCKER`: Helper/utils/common directory detected
- `UNAUTHORIZED_DIRECTORY_BLOCKER`: Directory created before its authorized phase

## 8. Future Phase Ownership

- **P0-C:** Authorized to create core/, tests/contracts/
- **P0-D:** Authorized to create tests/fakes/
- **P0-F:** Authorized to create platform/wasapi/
- **P0-G:** Authorized to create decode/, third_party/ (FFmpeg)
- **P0-H:** Authorized to create playback/

## 9. Deferred Items

- Automated directory authorization checking
- Content quality scoring for policy files

## 10. Content Requirements for Policy Files

Each policy file must contain at minimum:
1. Purpose
2. Scope
3. Current Phase Rule
4. Allowed
5. Forbidden
6. Gate / Check Method
7. Failure Classification
8. Future Phase Ownership
9. Deferred Items
10. Last Updated / Stage Source

A file with only titles, TODOs, or slogans FAILS this gate.

**Iron Rule 2:** Directories must grow by genealogy, not bucket directories, not empty shell forests.
**Iron Rule 3:** P0-B cannot go from empty directory forest to empty policy forest; policy must have substance.
