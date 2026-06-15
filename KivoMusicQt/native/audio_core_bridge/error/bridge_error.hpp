// =============================================================================
// Kivo Music Qt - bridge_error.hpp
// AudioCoreBridge: Error types and result handling
// =============================================================================

#pragma once

#include <QString>
#include <variant>

namespace kivo::qt::audio_bridge::error {

// Error categories
enum class BridgeErrorCategory {
    DllLoad,           // Failed to load kivo_audio_core.dll
    FunctionResolve,   // Failed to resolve ABI function
    EngineCreate,      // Failed to create audio engine
    SourceOpen,        // Failed to open audio source
    Command,           // Command execution failed
    Snapshot,          // Failed to read snapshot
    InvalidState,      // Invalid state transition
    Unknown
};

// Error information
struct BridgeError {
    BridgeErrorCategory category;
    QString message;
    int nativeCode = 0;  // Host ABI error code if available

    static BridgeError dllLoadFailed(const QString& path) {
        return {BridgeErrorCategory::DllLoad,
                QString("Failed to load DLL: %1").arg(path), 0};
    }

    static BridgeError functionResolveFailed(const QString& funcName) {
        return {BridgeErrorCategory::FunctionResolve,
                QString("Failed to resolve function: %1").arg(funcName), 0};
    }

    static BridgeError engineCreateFailed(int code) {
        return {BridgeErrorCategory::EngineCreate,
                QString("Failed to create audio engine"), code};
    }

    static BridgeError sourceOpenFailed(const QString& path, int code) {
        return {BridgeErrorCategory::SourceOpen,
                QString("Failed to open: %1").arg(path), code};
    }

    static BridgeError commandFailed(const QString& command, int code) {
        return {BridgeErrorCategory::Command,
                QString("Command failed: %1").arg(command), code};
    }

    static BridgeError invalidState(const QString& reason) {
        return {BridgeErrorCategory::InvalidState, reason, 0};
    }
};

// Result type for operations that may fail
template<typename T>
class BridgeResult {
public:
    static BridgeResult ok(T value) {
        return BridgeResult(std::move(value));
    }

    static BridgeResult failed(BridgeError error) {
        return BridgeResult(std::move(error));
    }

    bool isOk() const { return std::holds_alternative<T>(data_); }
    bool isFailed() const { return std::holds_alternative<BridgeError>(data_); }

    const T& value() const { return std::get<T>(data_); }
    T& value() { return std::get<T>(data_); }

    const BridgeError& error() const { return std::get<BridgeError>(data_); }

private:
    explicit BridgeResult(T value) : data_(std::move(value)) {}
    explicit BridgeResult(BridgeError error) : data_(std::move(error)) {}

    std::variant<T, BridgeError> data_;
};

// Void result for operations that don't return a value
using VoidResult = BridgeResult<bool>;

inline VoidResult success() {
    return VoidResult::ok(true);
}

inline VoidResult failure(BridgeError error) {
    return VoidResult::failed(std::move(error));
}

} // namespace kivo::qt::audio_bridge::error
