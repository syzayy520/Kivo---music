#pragma once

namespace kivo::core::contract {

/// @brief Contract for non-realtime boundaries.
///
/// This structure defines what operations are permitted when crossing
/// from a non-realtime context into a realtime context.
struct NonRealtimeBoundaryContract {
    /// May the caller allocate memory that will be used in realtime context?
    bool may_allocate{false};
    
    /// May the caller perform blocking I/O that will affect realtime context?
    bool may_block{false};
    
    /// May the caller log messages that will be processed in realtime context?
    bool may_log{false};
    
    /// May the caller invoke callbacks that will be executed in realtime context?
    bool may_callback{false};
    
    /// May the caller throw exceptions that will propagate into realtime context?
    bool may_throw{false};
    
    [[nodiscard]] bool operator==(const NonRealtimeBoundaryContract&) const noexcept = default;
};

} // namespace kivo::core::contract