// =============================================================================
// Kivo Music Qt - truth_snapshot.hpp
// AudioCoreBridge: Truth status projection from Host ABI snapshot
// =============================================================================

#pragma once

#include "../error/bridge_error.hpp"
#include "../loader/audio_core_function_resolver.hpp"
#include <QString>

namespace kivo::qt::audio_bridge::snapshot {

enum class BitPerfectStatus {
    Unknown,
    CoreReady,
    Active,
    Failed
};

struct TruthSnapshot {
    BitPerfectStatus bitPerfectStatus = BitPerfectStatus::Unknown;
    QString statusBadge() const;
};

class TruthSnapshotReader final {
public:
    static error::BridgeResult<TruthSnapshot> read(
        const loader::AudioCoreFunctions& functions,
        kivo_audio_handle engine);
};

} // namespace kivo::qt::audio_bridge::snapshot
