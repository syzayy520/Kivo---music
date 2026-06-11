#include "../../contract_tests_main.h"
#include "kivo/core/contract/realtime/path/realtime_path.hpp"
#include "kivo/core/contract/realtime/path/realtime_path_context.hpp"

using namespace kivo::core::contract;

void run_realtime_path_contract_tests(ContractTestRunner& runner) {
    runner.run("realtime_path_unknown_is_valid", []() {
        RealtimePath path{RealtimePath::Unknown};
        ASSERT(path == RealtimePath::Unknown);
        ASSERT(!(path == RealtimePath::Realtime));
        ASSERT(!(path == RealtimePath::NonRealtime));
    });
    
    runner.run("realtime_path_realtime_is_valid", []() {
        RealtimePath path{RealtimePath::Realtime};
        ASSERT(path == RealtimePath::Realtime);
        ASSERT(!(path == RealtimePath::Unknown));
        ASSERT(!(path == RealtimePath::NonRealtime));
    });
    
    runner.run("realtime_path_nonrealtime_is_valid", []() {
        RealtimePath path{RealtimePath::NonRealtime};
        ASSERT(path == RealtimePath::NonRealtime);
        ASSERT(!(path == RealtimePath::Unknown));
        ASSERT(!(path == RealtimePath::Realtime));
    });
    
    runner.run("realtime_path_enum_values", []() {
        ASSERT(static_cast<uint8_t>(RealtimePath::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(RealtimePath::Realtime) == 1);
        ASSERT(static_cast<uint8_t>(RealtimePath::NonRealtime) == 2);
    });
    
    runner.run("realtime_path_context_unknown_is_valid", []() {
        RealtimePathContext ctx{RealtimePathContext::Unknown};
        ASSERT(ctx == RealtimePathContext::Unknown);
        ASSERT(!(ctx == RealtimePathContext::RenderSide));
        ASSERT(!(ctx == RealtimePathContext::DecodeSide));
        ASSERT(!(ctx == RealtimePathContext::ControlSide));
        ASSERT(!(ctx == RealtimePathContext::BackgroundSide));
        ASSERT(!(ctx == RealtimePathContext::EventSide));
    });
    
    runner.run("realtime_path_context_render_side_is_valid", []() {
        RealtimePathContext ctx{RealtimePathContext::RenderSide};
        ASSERT(ctx == RealtimePathContext::RenderSide);
        ASSERT(!(ctx == RealtimePathContext::Unknown));
    });
    
    runner.run("realtime_path_context_decode_side_is_valid", []() {
        RealtimePathContext ctx{RealtimePathContext::DecodeSide};
        ASSERT(ctx == RealtimePathContext::DecodeSide);
        ASSERT(!(ctx == RealtimePathContext::Unknown));
    });
    
    runner.run("realtime_path_context_control_side_is_valid", []() {
        RealtimePathContext ctx{RealtimePathContext::ControlSide};
        ASSERT(ctx == RealtimePathContext::ControlSide);
        ASSERT(!(ctx == RealtimePathContext::Unknown));
    });
    
    runner.run("realtime_path_context_background_side_is_valid", []() {
        RealtimePathContext ctx{RealtimePathContext::BackgroundSide};
        ASSERT(ctx == RealtimePathContext::BackgroundSide);
        ASSERT(!(ctx == RealtimePathContext::Unknown));
    });
    
    runner.run("realtime_path_context_event_side_is_valid", []() {
        RealtimePathContext ctx{RealtimePathContext::EventSide};
        ASSERT(ctx == RealtimePathContext::EventSide);
        ASSERT(!(ctx == RealtimePathContext::Unknown));
    });
    
    runner.run("realtime_path_context_enum_values", []() {
        ASSERT(static_cast<uint8_t>(RealtimePathContext::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(RealtimePathContext::RenderSide) == 1);
        ASSERT(static_cast<uint8_t>(RealtimePathContext::DecodeSide) == 2);
        ASSERT(static_cast<uint8_t>(RealtimePathContext::ControlSide) == 3);
        ASSERT(static_cast<uint8_t>(RealtimePathContext::BackgroundSide) == 4);
        ASSERT(static_cast<uint8_t>(RealtimePathContext::EventSide) == 5);
    });
}