# Future Phase Taskbooks

## Execution Authority

Before selecting a taskbook, read:

```text
docs/architecture/audio_core_execution_roadmap.md
```

That roadmap defines the active implementation sequence. These taskbooks
provide phase-local requirements and do not independently authorize skipping,
reordering, or expanding milestones.

Read these taskbooks before planning future work:

```text
P0-C-011  docs/tasks/p0_c_011_playback_state_machine_semantics_family_design_task.md
P0-C-012  docs/tasks/p0_c_012_command_semantics_family_design_task.md
P0-C-013  docs/tasks/p0_c_013_observability_contract_family_design_task.md
P0-C-014  docs/tasks/p0_c_014_policy_boundary_family_design_task.md
P0-D      docs/tasks/p0_d_fault_injection_backend_family_design_task.md
P0-E      docs/tasks/p0_e_render_boundary_failure_first_family_design_task.md
P0-F      docs/tasks/p0_f_platform_output_adapter_shell_family_design_task.md
P0-G      docs/tasks/p0_g_decode_seam_family_design_task.md
P0-H      docs/tasks/p0_h_minimum_real_output_family_design_task.md
P0-I      docs/tasks/p0_i_decode_to_output_loop_family_design_task.md
P0-K      docs/tasks/p0_k_commercial_stability_matrix_family_design_task.md
P0-L      docs/tasks/p0_l_gapless_timeline_accuracy_family_design_task.md
P0-M      docs/tasks/p0_m_exclusive_bit_perfect_truth_family_design_task.md
P0-N      docs/tasks/p0_n_volume_processing_boundary_family_design_task.md
P0-O      docs/tasks/p0_o_host_abi_integration_boundary_family_design_task.md
P0-P      docs/tasks/p0_p_release_hardening_family_design_task.md
P0-Q      docs/tasks/p0_q_hi_res_dsd_dop_research_family_design_task.md
P0-R      docs/tasks/p0_r_mpv_libmpv_reference_research_family_design_task.md
PROGRAM   docs/architecture/windows_backend_completion_program.md
LYRICS    docs/architecture/lyrics_core_boundary.md
```

Rule:

```text
active execution roadmap first
planning draft first
owner review second
implementation only after approval
implementation evidence before additional planning expansion
```
