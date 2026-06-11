#include "../../contract_tests_main.h"
#include "kivo/core/contract/error/domain/error_domain.hpp"

using namespace kivo::core::contract;

namespace {

void error_domain_default_is_unknown(ContractTestRunner& runner) {
    runner.run("error_domain_default_is_unknown", []() {
        ErrorDomain d{};
        ASSERT(d == ErrorDomain::Unknown);
    });
}

void error_domain_user_error(ContractTestRunner& runner) {
    runner.run("error_domain_user_error", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::UserError) == 1);
    });
}

void error_domain_media_error(ContractTestRunner& runner) {
    runner.run("error_domain_media_error", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::MediaError) == 2);
    });
}

void error_domain_codec_error(ContractTestRunner& runner) {
    runner.run("error_domain_codec_error", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::CodecError) == 3);
    });
}

void error_domain_format_error(ContractTestRunner& runner) {
    runner.run("error_domain_format_error", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::FormatError) == 4);
    });
}

void error_domain_device_error(ContractTestRunner& runner) {
    runner.run("error_domain_device_error", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::DeviceError) == 5);
    });
}

void error_domain_backend_error(ContractTestRunner& runner) {
    runner.run("error_domain_backend_error", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::BackendError) == 6);
    });
}

void error_domain_host_error(ContractTestRunner& runner) {
    runner.run("error_domain_host_error", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::HostError) == 7);
    });
}

void error_domain_internal_bug(ContractTestRunner& runner) {
    runner.run("error_domain_internal_bug", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::InternalBug) == 8);
    });
}

void error_domain_device_lost(ContractTestRunner& runner) {
    runner.run("error_domain_device_lost", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::DeviceLost) == 9);
    });
}

void error_domain_format_changed(ContractTestRunner& runner) {
    runner.run("error_domain_format_changed", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::FormatChanged) == 10);
    });
}

void error_domain_codec_starved(ContractTestRunner& runner) {
    runner.run("error_domain_codec_starved", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::CodecStarved) == 11);
    });
}

void error_domain_buffer_underrun(ContractTestRunner& runner) {
    runner.run("error_domain_buffer_underrun", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::BufferUnderrun) == 12);
    });
}

void error_domain_buffer_overrun(ContractTestRunner& runner) {
    runner.run("error_domain_buffer_overrun", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::BufferOverrun) == 13);
    });
}

void error_domain_unsupported_format(ContractTestRunner& runner) {
    runner.run("error_domain_unsupported_format", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::UnsupportedFormat) == 14);
    });
}

void error_domain_source_unavailable(ContractTestRunner& runner) {
    runner.run("error_domain_source_unavailable", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::SourceUnavailable) == 15);
    });
}

void error_domain_network_interrupted(ContractTestRunner& runner) {
    runner.run("error_domain_network_interrupted", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::NetworkInterrupted) == 16);
    });
}

void error_domain_permission_denied(ContractTestRunner& runner) {
    runner.run("error_domain_permission_denied", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::PermissionDenied) == 17);
    });
}

void error_domain_cache_invalid(ContractTestRunner& runner) {
    runner.run("error_domain_cache_invalid", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomain::CacheInvalid) == 18);
    });
}

void error_domain_equality(ContractTestRunner& runner) {
    runner.run("error_domain_equality", []() {
        ASSERT(ErrorDomain::CodecError == ErrorDomain::CodecError);
        ASSERT(ErrorDomain::Unknown != ErrorDomain::CodecError);
    });
}

} // namespace

void run_error_domain_tests(ContractTestRunner& runner) {
    error_domain_default_is_unknown(runner);
    error_domain_user_error(runner);
    error_domain_media_error(runner);
    error_domain_codec_error(runner);
    error_domain_format_error(runner);
    error_domain_device_error(runner);
    error_domain_backend_error(runner);
    error_domain_host_error(runner);
    error_domain_internal_bug(runner);
    error_domain_device_lost(runner);
    error_domain_format_changed(runner);
    error_domain_codec_starved(runner);
    error_domain_buffer_underrun(runner);
    error_domain_buffer_overrun(runner);
    error_domain_unsupported_format(runner);
    error_domain_source_unavailable(runner);
    error_domain_network_interrupted(runner);
    error_domain_permission_denied(runner);
    error_domain_cache_invalid(runner);
    error_domain_equality(runner);
}