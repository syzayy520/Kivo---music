# P2 Execution Plan — Audio-Truth Surface (adversarially vetted)

**Source:** `kivo-p2-design` workflow (13 agents; truth-honesty / ABI-contract /
boundary-realtime lenses). **ABI:** **1.1.0 → 1.2.0** (additive). **Crux finding:**
the bit-perfect evidence pipeline (`build_bit_perfect_evidence`,
`evaluate_bit_perfect_truth`) is **dead code** — referenced only by tests. P2 wires
it into the runtime snapshot, exposes it flat over the ABI via a **dedicated
`kivo_audio_truth_v1` struct + `kivo_audio_get_truth` getter** (NOT the hot
snapshot tail), and binds an HONEST QML panel.

## Honesty law (audio_truth_policy Iron Rules 20/21/37/38)
"Show **Unknown** when evidence incomplete OR capability absent" enforced at every
layer. The verdict word "Bit-Perfect" appears ONLY when
`isBitPerfectConfirmed() = evidenceComplete && verdict==BitPerfect &&
actualMode==Exclusive && engineInPath==No && conversionActive==No &&
processingActive==No` (defensive vs an incoherent flat tail). Sample-rate/bit-depth
shown only when `evidenceComplete`. Tri-states are Yes/No/**Unknown** (a bare 0 must
never read as a favorable "No"). Conservative proxies (documented, test-locked):
`host_audio_engine_in_path=false`, `policy_allows_bit_perfect = requested==Exclusive`,
`device_format==render_format` valid only under Exact format policy — all can only
REJECT/mark-incomplete, never falsely assert.

## Steps

### Step 1 — Core: wire dead evidence/verdict into the runtime snapshot
- `runtime/state/playback_runtime_coordinator_state.hpp`: add Impl members (captured
  under `mutex_`): `source_format_`, `device_format_` (AudioFormatDescriptor),
  `conversion_snapshot_` (AudioConversionSnapshot), `requested_output_mode_`,
  `actual_output_mode_` (RenderOutputMode), `policy_allows_bit_perfect_` (bool).
- `runtime/open/playback_runtime_open.cpp`: in the success `scoped_lock` (by the P1
  timebase block ~L154): `source_format_ = decode_open.probe().native_format.format;
  device_format_ = format_.format; conversion_snapshot_ = probe().conversion_snapshot;
  requested_/actual_output_mode_ = render_request.output_mode;
  policy_allows_bit_perfect_ = (output_mode==Exclusive)`. Comment the equivalences.
- `runtime/resource/playback_runtime_resource.cpp`: reset the 6 members in
  `release_runtime()` next to `format_={}` (no stale verdict leak).
- `snapshot/playback_runtime_snapshot.hpp`: embed `output::BitPerfectEvidence
  bit_perfect_evidence{}` + `contract::BitPerfectTruthReport bit_perfect_report{}`
  (add a defaulted `operator==` to BitPerfectEvidence so the snapshot still compiles).
- `snapshot/playback_runtime_snapshot.cpp`: when `active_`, `build_bit_perfect_evidence(
  requested_,actual_,source_,format_.format,device_,/*engine_in_path=*/false,
  policy_,conversion_snapshot_, producer_?producer_->snapshot().processing:{})` then
  `evaluate_bit_perfect_truth(...)`; store both. Off the realtime thread (snapshot()
  is caller-path, under `mutex_`; builder/evaluator are noexcept/alloc-free).

### Step 2 — ABI: dedicated flat truth struct + version/capability
- NEW `include/kivo/host/abi/snapshot/kivo_audio_truth.h` (C99/stdint only, no
  C++/optional): `enum kivo_audio_bit_perfect_verdict {UNKNOWN=0,BIT_PERFECT=1,
  REJECTED=2,EVIDENCE_INCOMPLETE=3}`; rejection-category enum mirroring
  `BitPerfectRejectionCategory` (freeze order w/ static_assert + comment in BOTH
  headers); 16-byte `kivo_audio_format_descriptor {u32 sample_rate; u32 channel_mask;
  u16 channel_count; u8 sample_format; u8 bits_per_sample; u8 container_bits; u8
  frame_layout; u16 _pad}`; `kivo_audio_truth_v1 {u32 struct_size; u32 struct_version;
  u64 known_mask; u64 bool_mask; u32 verdict; u32 rejection_category; u32
  evidence_complete; u32 requested_mode; u32 actual_mode; u32 _pad; format src/render/
  device}`. Pin each known_mask/bool_mask bit index with named enum + static_assert.
  Mode: raw u32 0=Shared/1=Exclusive/0xFFFFFFFF=unknown (do NOT redefine
  `kivo_audio_output_mode` — it already exists). `KIVO_AUDIO_TRUTH_V1_BASE_SIZE =
  sizeof(kivo_audio_truth_v1)`.
- `version/kivo_audio_abi_version.h`: `MINOR 1u→2u`. struct_version stays 1.
- `capability/kivo_audio_capabilities.h`: `KIVO_AUDIO_CAPABILITY_BIT_PERFECT_TRUTH =
  1ull<<10`.
- `api/kivo_audio_api.h`: include the new header + declare `kivo_audio_get_truth`.

### Step 3 — Host: getter + flattener + export + capability
- NEW `src/core/output/truth/encoding/bit_perfect_truth_flattener.hpp` (core-side,
  ABI-FREE: must NOT include kivo_audio_truth.h): pure fn evidence+report → POD
  outputs (known_mask, bool_mask, verdict int, rejection int, complete, modes, 3 flat
  format PODs). Unit-testable without the host DLL.
- `host/abi/engine/host_audio_engine.hpp`: declare `truth(void*,uint32_t) noexcept`.
- NEW `host/abi/engine/host_audio_engine_truth.cpp`: `scoped_lock{command_mutex_}`;
  `runtime_.snapshot()`; flatten into `kivo_audio_truth_v1`; verdict map; `min`-memcpy.
- `host/abi/api/host_audio_api.cpp`: export `kivo_audio_get_truth` (validate prefix).
- `host/abi/engine/host_audio_engine_lifecycle.cpp`: OR the capability bit.
- `host/abi/export/kivo_audio_core.def`: **add `kivo_audio_get_truth` to EXPORTS**.
- `CMakeLists.txt`: add `host_audio_engine_truth.cpp` to `kivo_host_abi`.

### Step 4 — Bridge: resolve optional getter, structured verdict-gated properties
- resolver: add `getTruth` (OPTIONAL — null on a 1.1.0 core → Unknown).
- `snapshot/truth_snapshot.{hpp,cpp}`: keep coarse `lifecycleBadge()`; add structured
  fields + the honesty chokepoint helpers (`isBitPerfectConfirmed()`, `verdictText()`,
  `sampleRateText()`/`bitDepthText()` gated on `evidenceComplete`, tri-state text,
  `rejectionText()`). Reader gains a `truthCapabilitySupported` 3rd param.
- `audio_playback_controller.{hpp,cpp}`: KEEP `bitPerfectStatus` (now lifecycleBadge);
  add read-only truth Q_PROPERTYs + one coalesced `truthChanged()`.
- `controller/audio_playback_controller_impl*.{hpp,cpp}`: `truthSnapshotSupported_`
  (set in lifecycle from caps), `lastTruthSnapshot_`, getters in `_properties.cpp`,
  poll/store/emit in `_snapshot.cpp` (emit truthChanged OUTSIDE stateMutex_).

### Step 5 — QML: bind, instantiate reachably, delete fakes
- `components/truth/TruthBadge.qml`: generic chip, Theme tokens, Unknown-coercion;
  verdict tone gold ONLY when value==='Bit-Perfect' in this one place.
- NEW `components/truth/TruthRow.qml`: label/value/tone/known row primitive.
- `components/truth/SignalPathPanel.qml`: full rewrite — delete hardcoded literals;
  bind rows to controller truth props; container-bits row separate; engine/conversion/
  processing neutral-toned; whole block gated on `truthEvidenceComplete`.
- NEW `components/truth/SignalPathPopover.qml`: Popup wrapping the panel.
- NEW `components/transport/PlayingPulse.qml`: binary playing indicator (no fake data).
- `components/transport/TransportBar.qml`: delete the RNG visualizer rows → PlayingPulse
  + a button toggling SignalPathPopover.
- `App.qml` ~L275: remove the unevidenced "bit-perfect" marketing word.

## Governance
Genealogy: new `src/core/output/truth/encoding/` + `host/abi/engine/host_audio_engine_truth.cpp`
+ QML `components/truth/*` — register any new core subfamily in `check_genealogy_gate.ps1`.
No-empty-forest: backend getter + frontend panel land together. Forbidden-token: the
flattener stays ABI-free; core gains no Qt/Windows/avcodec. ABI: dedicated struct keeps
the frozen snapshot prefix → `legacy_snapshot_prefix_is_not_overwritten` stays green;
add a new compatibility/contract test for `kivo_audio_get_truth` + the flattener.
