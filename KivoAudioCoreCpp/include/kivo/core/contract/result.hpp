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
#include <utility>
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
        return StatusOr(OkTag{}, std::move(value));
    }

    static StatusOr Error(std::string message) {
        return StatusOr(ErrorTag{}, std::move(message));
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
        return std::get<0>(data_);
    }

    [[nodiscard]] std::string_view error() const {
        return std::get<1>(data_).message;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const StatusOr&) const = default;

private:
    struct OkTag {};
    struct ErrorTag {};

    StatusOr(OkTag, T value)
        : result_(Result::Ok),
          data_(std::in_place_index<0>, std::move(value)) {}

    StatusOr(ErrorTag, std::string message)
        : result_(Result::Error),
          data_(std::in_place_index<1>, ErrorInfo{std::move(message)}) {}

    Result result_;
    std::variant<T, ErrorInfo> data_;
};

} // namespace kivo::core::contract
