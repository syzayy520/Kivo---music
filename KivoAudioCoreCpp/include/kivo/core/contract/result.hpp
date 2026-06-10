// =============================================================================
// KivoAudioCoreCpp — result.hpp
// Contract: Result/Status types for Kivo audio core
// =============================================================================
//
// Minimal error handling primitives shared across all contract families.
// Platform-neutral. No runtime dependencies.
// =============================================================================

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

namespace kivo::core::contract {

// =============================================================================
// Result — Binary outcome enum
// =============================================================================
enum class [[nodiscard]] Result : uint8_t {
    Ok = 0,
    Error = 1
};

// =============================================================================
// ErrorInfo — Error message wrapper (avoids variant ambiguity when T=string)
// =============================================================================
struct ErrorInfo {
    std::string message;

    [[nodiscard]] bool operator==(const ErrorInfo&) const noexcept = default;
};

// =============================================================================
// StatusOr<T> — Value-or-error wrapper
// =============================================================================
template<typename T>
class [[nodiscard]] StatusOr {
public:
    // --- Factory methods ---
    static StatusOr Ok(T value) {
        StatusOr s;
        s.result_ = Result::Ok;
        s.data_ = std::move(value);
        return s;
    }

    static StatusOr Error(std::string message) {
        StatusOr s;
        s.result_ = Result::Error;
        s.data_ = ErrorInfo{std::move(message)};
        return s;
    }

    // --- Observers ---
    [[nodiscard]] bool is_ok() const noexcept {
        return result_ == Result::Ok;
    }

    [[nodiscard]] bool is_error() const noexcept {
        return result_ == Result::Error;
    }

    [[nodiscard]] Result result() const noexcept {
        return result_;
    }

    // --- Accessors (precondition: is_ok() / is_error()) ---
    [[nodiscard]] const T& value() const {
        return std::get<T>(data_);
    }

    [[nodiscard]] std::string_view error() const {
        return std::get<ErrorInfo>(data_).message;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const StatusOr&) const noexcept = default;

private:
    StatusOr() = default;

    Result result_ = Result::Error;
    std::variant<T, ErrorInfo> data_;
};

} // namespace kivo::core::contract
