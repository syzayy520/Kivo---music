// =============================================================================
// Kivo Music Qt - audio_core_worker_result.hpp
// Worker result types: success/failure payloads sent back to controller
// =============================================================================

#pragma once

#include <QString>

namespace kivo::qt::audio_bridge::worker {

struct WorkerResult {
    bool ok = false;
    QString label;
    int resultCode = 0;
    QString message;

    static WorkerResult success(const QString& label) {
        return {true, label, 0, {}};
    }
    static WorkerResult failure(const QString& label, int code, const QString& msg) {
        return {false, label, code, msg};
    }
};

} // namespace kivo::qt::audio_bridge::worker
