#pragma once

// Test header to verify all dependencies are available

// fmt library
#include <fmt/format.h>
#include <fmt/color.h>

// JSON library
#include <nlohmann/json.hpp>

// MinHook library (for API hooking)
#include <MinHook.h>

// Zydis library (for disassembly)
#include <Zydis/Zydis.h>

namespace vision {
namespace common {

// Simple dependency verification functions
inline bool verify_dependencies() {
    // Test fmt
    auto test_fmt = fmt::format("fmt: {}", "OK");
    
    // Test JSON
    nlohmann::json test_json = {{"test", "OK"}};
    
    // Test MinHook initialization (without actually hooking)
    MH_STATUS mh_status = MH_Initialize();
    bool minhook_ok = (mh_status == MH_OK || mh_status == MH_ERROR_ALREADY_INITIALIZED);
    if (mh_status == MH_OK) {
        MH_Uninitialize();
    }
    
    // Test Zydis decoder initialization
    ZydisDecoder decoder;
    bool zydis_ok = ZYAN_SUCCESS(ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LEGACY_32, ZYDIS_STACK_WIDTH_32));
    
    return !test_fmt.empty() && !test_json.empty() && minhook_ok && zydis_ok;
}

} // namespace common
} // namespace vision
