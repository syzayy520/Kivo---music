# Realtime Thread and State Policy

**Purpose:** Define rules for realtime audio threads, command/event queues, and state management.  
**Scope:** All phases from P0-C onward.  
**Current Phase Rule:** P0-B has no threads, queues, or runtime state.  
**Last Updated:** P0-B (V10.1)  

## 1. Purpose

This policy governs the realtime audio rendering thread, command/event queue semantics, and state machine design. It prevents common realtime audio pitfalls: priority inversion, unbounded allocation, blocking calls, and hidden state.

## 2. Scope

Applies to all future audio rendering threads, playback state machines, and command/event systems within KivoAudioCoreCpp/.

## 3. Current Phase Rule (P0-B)

P0-B has no runtime, no threads, no queues, no state machines. This policy documents the rules for P0-C+.

## 4. Allowed

- Documentation describing thread architecture
- Policy documents referencing command/event semantics
- Architecture diagrams showing thread boundaries
- Bounded command/event queues (P0-C+)
- Lock-free queue implementations (P0-C+)
- Generation-based stale event discarding (P0-C+)

## 5. Forbidden

- Unbounded queues on realtime path
- Dynamic memory allocation on realtime audio path
- Blocking I/O on render thread
- File I/O on render thread
- UI callbacks on render thread
- High-frequency logging on render thread
- Long-held locks on render thread
- Uncontrolled mutable globals shared across threads
- Hidden singleton state
- FFmpeg calls on render thread
- Decode operations on render thread

## 6. Gate / Check Method

- Manual review for P0-B (no runtime to gate)
- Future: automated thread annotation scanning
- Future: lock-free queue validation

## 7. Failure Classification

- `REALTIME_ALLOCATION_BLOCKER`: Dynamic allocation on realtime path
- `REALTIME_BLOCKING_CALL_BLOCKER`: Blocking call on render thread
- `REALTIME_IO_BLOCKER`: File/network I/O on render thread
- `THREAD_SAFETY_VIOLATION_BLOCKER`: Shared state without synchronization
- `QUEUE_UNBOUNDED_BLOCKER`: Unbounded queue on critical path

## 8. Future Phase Ownership

- **P0-C:** Define command/event contract types
- **P0-H:** Implement render thread with bounded queues
- **P0-J:** State machine hardening and error recovery
- **P0-K:** Device matrix testing under load

## 9. Deferred Items

- Thread sanitizer integration
- Lock-free queue library selection
- Realtime priority scheduling policy
- CPU affinity configuration

## 10. Command/Event Semantics (Preview)

- Commands: Host -> Engine (play, pause, seek, stop)
- Events: Engine -> Host (state change, error, position update)
- All command/event must be orderable, trackable, and stale-generation-discardable

**Iron Rule 33:** All command/event must be orderable, trackable, and discardable stale generation.
**Iron Rule 47:** Audio queue must be bounded; buffer ownership must be explicit; generation discard must be testable.
