// =============================================================================
// Kivo Music Qt - format_snapshot.hpp
// AudioCoreBridge: Format text projection from Host ABI snapshot
// =============================================================================

#pragma once

#include "../error/bridge_error.hpp"
#include "../loader/audio_core_function_resolver.hpp"
#include <QString>

namespace kivo::qt::audio_bridge::snapshot {

struct FormatSnapshot {
    QString text = "Core ready";

    QString formatText() const;
};

class FormatSnapshotReader final {
public:
    static error::BridgeResult<FormatSnapshot> read(
        const loader::AudioCoreFunctions& functions,
        kivo_audio_handle engine);
};

} // namespace kivo::qt::audio_bridge::snapshot
