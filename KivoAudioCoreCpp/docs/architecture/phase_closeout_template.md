# Phase Closeout Template

This template defines the required structure for phase closeout reports.

---

## [PHASE_ID] Closeout

**Task:** [FULL_TASK_NAME]  
**Phase:** [PHASE_LETTER]  
**Date:** [ISO_DATE]  
**Execution Basis:** [DESIGN_DOC_VERSION]  

### 1. PROJECT_ROOT

- **PROJECT_ROOT:** [ABSOLUTE_PATH]
- **Repo Root:** [REPO_ROOT_PATH]
- **Independent Repo:** [YES/NO]
- **Monorepo Subdirectory:** [YES/NO]

### 2. Execution Scope

- **Files Created:** [LIST]
- **Files Modified:** [LIST]
- **Files Outside PROJECT_ROOT Modified:** NONE

### 3. Gate Results

| Gate | Result | Details |
|------|--------|---------|
| Forbidden Token | [PASS/FAIL] | [DETAILS] |
| Genealogy | [PASS/FAIL] | [DETAILS] |
| No Empty Forest | [PASS/FAIL] | [DETAILS] |
| No Audio Runtime | [PASS/FAIL] | [DETAILS] |
| Policy Substance | [PASS/FAIL] | [DETAILS] |
| Dependency/License | [PASS/FAIL] | [DETAILS] |
| Toolchain/Configure | [PASS/FAIL/ENVIRONMENT_BLOCKED] | [DETAILS] |

### 4. Git Status

- **Branch:** [BRANCH_NAME]
- **Working Tree:** [CLEAN/DIRTY]
- **Local HEAD:** [40_CHAR_SHA]
- **Remote HEAD:** [40_CHAR_SHA/NO_REMOTE]
- **Push Status:** [PUSHED/NO_PUSH/BLOCKED]

### 5. Policy Substance

| Policy | Exists | Has Required Sections | Non-Placeholder |
|--------|--------|-----------------------|-----------------|
| toolchain_and_build_policy.md | [Y/N] | [Y/N] | [Y/N] |
| dependency_and_license_policy.md | [Y/N] | [Y/N] | [Y/N] |
| architecture_boundary_policy.md | [Y/N] | [Y/N] | [Y/N] |
| realtime_thread_and_state_policy.md | [Y/N] | [Y/N] | [Y/N] |
| audio_truth_policy.md | [Y/N] | [Y/N] | [Y/N] |
| validation_and_release_policy.md | [Y/N] | [Y/N] | [Y/N] |
| no_empty_forest_policy.md | [Y/N] | [Y/N] | [Y/N] |

### 6. Go / No-Go

**Conclusion:** [GO/NO_GO_CLASSIFICATION]

**Reasoning:** [EXPLANATION]

### 7. Risk / Deferred Items

- [ITEM_1]
- [ITEM_2]

### 8. Next Phase Input

[WHAT_THE_NEXT_PHASE_NEEDS_FROM_THIS_PHASE]

---

**Classification:** [FINAL_CLASSIFICATION]
