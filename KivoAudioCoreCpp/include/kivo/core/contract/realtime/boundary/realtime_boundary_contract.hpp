#pragma once

#include "../path/realtime_path.hpp"
#include "../path/realtime_path_context.hpp"
#include "../prohibition/realtime_allocation_rule.hpp"
#include "../prohibition/realtime_blocking_io_rule.hpp"
#include "../prohibition/realtime_locking_rule.hpp"
#include "../prohibition/realtime_logging_rule.hpp"
#include "../prohibition/realtime_callback_rule.hpp"
#include "../prohibition/realtime_exception_rule.hpp"
#include "../buffer/preallocated_buffer_rule.hpp"
#include "../buffer/buffer_ownership_rule.hpp"
#include "../buffer/buffer_lifetime_rule.hpp"
#include "../ownership/ownership_transfer_rule.hpp"
#include "../ownership/ownership_visibility_rule.hpp"
#include "../transport/bounded_transport_rule.hpp"
#include "../transport/spsc_semantics_rule.hpp"

namespace kivo::core::contract {

/// @brief Aggregated realtime boundary contract.
///
/// Combines all realtime contract rules into a single structure
/// that can be validated at compile-time or during static analysis.
struct RealtimeBoundaryContract {
    /// The realtime path classification.
    RealtimePath path{RealtimePath::Unknown};
    
    /// The realtime path context (side of the boundary).
    RealtimePathContext path_context{RealtimePathContext::Unknown};
    
    /// Memory allocation constraints.
    AllocationConstraint allocation{AllocationConstraint::NotSpecified};
    
    /// Blocking I/O constraints.
    BlockingIoConstraint blocking_io{BlockingIoConstraint::NotSpecified};
    
    /// Locking constraints.
    LockingConstraint locking{LockingConstraint::NotSpecified};
    
    /// Logging constraints.
    LoggingConstraint logging{LoggingConstraint::NotSpecified};
    
    /// Callback constraints.
    CallbackConstraint callback{CallbackConstraint::NotSpecified};
    
    /// Exception constraints.
    ExceptionConstraint exception{ExceptionConstraint::NotSpecified};
    
    /// Buffer preallocation requirements.
    PreallocationRequirement preallocation{PreallocationRequirement::NotSpecified};
    
    /// Bounded transport requirements.
    BoundedTransport transport{BoundedTransport::NotSpecified};
    
    /// SPSC semantics declaration.
    SpscSemantics spsc{SpscSemantics::NotSpecified};
    
    /// Ownership transfer constraints.
    OwnershipTransfer ownership_transfer{OwnershipTransfer::NotSpecified};
    
    /// Ownership visibility requirements.
    OwnershipVisibility ownership_visibility{OwnershipVisibility::NotSpecified};
    
    /// Buffer ownership constraints.
    BufferOwnership buffer_ownership{BufferOwnership::NotSpecified};
    
    /// Buffer lifetime proof requirements.
    BufferLifetimeProof buffer_lifetime{BufferLifetimeProof::NotSpecified};
    
    [[nodiscard]] bool operator==(const RealtimeBoundaryContract&) const noexcept = default;
    
    /// @brief Validates contract-state consistency.
    ///
    /// This is a minimal constexpr check for contract-state consistency only.
    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return path != RealtimePath::Unknown && 
               path_context != RealtimePathContext::Unknown;
    }
};

} // namespace kivo::core::contract