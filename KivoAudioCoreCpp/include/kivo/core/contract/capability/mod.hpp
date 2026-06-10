// =============================================================================
// KivoAudioCoreCpp — mod.hpp
// Contract: Capability module — includes all capability contract types
// =============================================================================
//
// This is the single entry point for the capability contract family.
// Include this header to access all capability types.
// =============================================================================

#pragma once

// Identity
#include "identity/scope.hpp"
#include "identity/subject.hpp"
#include "identity/source.hpp"

// Quality
#include "quality/level.hpp"
#include "quality/confidence.hpp"
#include "quality/path.hpp"

// Constraint
#include "constraint/strictness.hpp"
#include "constraint/memory.hpp"
#include "constraint/buffer.hpp"

// Component
#include "component/profile.hpp"
#include "component/requirement.hpp"
#include "component/hdr.hpp"
#include "component/decode.hpp"
#include "component/render.hpp"
#include "component/output.hpp"

// Domain
#include "domain/source_caps.hpp"
#include "domain/clock.hpp"
#include "domain/thread.hpp"
#include "domain/conversion.hpp"

// Negotiation
#include "negotiation/candidate.hpp"
#include "negotiation/candidate_presence.hpp"
#include "negotiation/result.hpp"
#include "negotiation/constraints.hpp"
#include "negotiation/comparison.hpp"
