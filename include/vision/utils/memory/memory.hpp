#pragma once

#include <vision/result.hpp>
#include <vision/utils/memory/address.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

#ifdef WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

namespace vision::utils::memory {

class c_memory {
public:
    // Constructors
    c_memory();  // Main module
    explicit c_memory(std::string_view module_name);
    
    // Non-copyable, movable
    c_memory(const c_memory&) = delete;
    c_memory& operator=(const c_memory&) = delete;
    c_memory(c_memory&&) noexcept = default;
    c_memory& operator=(c_memory&&) noexcept = default;
    
    // Destructor
    ~c_memory() = default;
    
    // Pattern scanning
    [[nodiscard]] result<c_address_internal> find_pattern(std::string_view pattern) const;
    [[nodiscard]] result<std::vector<c_address_internal>> find_all_patterns(std::string_view pattern) const;
    
    // String searching
    [[nodiscard]] result<c_address_internal> find_string(std::string_view str) const;
    [[nodiscard]] result<std::vector<c_address_internal>> find_all_strings(std::string_view str) const;
    
    // Export resolution
    [[nodiscard]] result<c_address_internal> get_export(std::string_view export_name) const;
    
    // Module information
    [[nodiscard]] std::uintptr_t base_address() const { return m_base_address; }
    [[nodiscard]] std::size_t size() const { return m_size; }
    [[nodiscard]] const std::string& name() const { return m_module_name; }
    [[nodiscard]] bool is_valid() const { return m_base_address != 0 && m_size > 0; }

private:
    // Pattern parsing and matching
    [[nodiscard]] std::vector<std::uint8_t> parse_pattern(std::string_view pattern) const;
    [[nodiscard]] std::vector<bool> parse_mask(std::string_view pattern) const;
    [[nodiscard]] bool pattern_matches(std::uintptr_t address, 
                                      const std::vector<std::uint8_t>& pattern,
                                      const std::vector<bool>& mask) const;
    
    // Module resolution
    [[nodiscard]] result<void> initialize_module(std::string_view module_name);
    
    std::string m_module_name;
    std::uintptr_t m_base_address = 0;
    std::size_t m_size = 0;
    HMODULE m_module_handle = nullptr;
};

// Implementation details
inline c_memory::c_memory() {
    auto init_result = initialize_module("");
    if (!init_result) {
        // Log error but don't throw - let is_valid() handle it
    }
}

inline c_memory::c_memory(std::string_view module_name) {
    auto init_result = initialize_module(module_name);
    if (!init_result) {
        // Log error but don't throw - let is_valid() handle it
    }
}

inline result<void> c_memory::initialize_module(std::string_view module_name) {
    if (module_name.empty()) {
        // Get main module
        m_module_handle = ::GetModuleHandleA(nullptr);
        m_module_name = "main";
    } else {
        m_module_handle = ::GetModuleHandleA(module_name.data());
        m_module_name = module_name;
    }
    
    if (!m_module_handle) {
        return result<void>::error("Failed to get module handle for: " + std::string(module_name));
    }
    
    // Get module information
    MODULEINFO module_info{};
    if (!::GetModuleInformation(::GetCurrentProcess(), m_module_handle, &module_info, sizeof(module_info))) {
        return result<void>::error("Failed to get module information");
    }
    
    m_base_address = reinterpret_cast<std::uintptr_t>(module_info.lpBaseOfDll);
    m_size = module_info.SizeOfImage;
    
    return result<void>::ok();
}

inline result<c_address_internal> c_memory::find_pattern(std::string_view pattern) const {
    if (!is_valid()) {
        return result<c_address_internal>::error("Invalid module");
    }
    
    const auto parsed_pattern = parse_pattern(pattern);
    const auto mask = parse_mask(pattern);
    
    if (parsed_pattern.empty()) {
        return result<c_address_internal>::error("Invalid pattern format");
    }
    
    for (std::uintptr_t addr = m_base_address; addr < m_base_address + m_size - parsed_pattern.size(); ++addr) {
        if (pattern_matches(addr, parsed_pattern, mask)) {
            return result<c_address_internal>::ok(c_address_internal(addr));
        }
    }
    
    return result<c_address_internal>::error("Pattern not found");
}

inline result<std::vector<c_address_internal>> c_memory::find_all_patterns(std::string_view pattern) const {
    if (!is_valid()) {
        return result<std::vector<c_address_internal>>::error("Invalid module");
    }
    
    const auto parsed_pattern = parse_pattern(pattern);
    const auto mask = parse_mask(pattern);
    
    if (parsed_pattern.empty()) {
        return result<std::vector<c_address_internal>>::error("Invalid pattern format");
    }
    
    std::vector<c_address_internal> results;
    
    for (std::uintptr_t addr = m_base_address; addr < m_base_address + m_size - parsed_pattern.size(); ++addr) {
        if (pattern_matches(addr, parsed_pattern, mask)) {
            results.emplace_back(addr);
        }
    }
    
    if (results.empty()) {
        return result<std::vector<c_address_internal>>::error("No patterns found");
    }
    
    return result<std::vector<c_address_internal>>::ok(std::move(results));
}

inline result<c_address_internal> c_memory::find_string(std::string_view str) const {
    if (!is_valid()) {
        return result<c_address_internal>::error("Invalid module");
    }
    
    if (str.empty()) {
        return result<c_address_internal>::error("Empty search string");
    }
    
    const auto* search_data = reinterpret_cast<const char*>(m_base_address);
    const auto search_size = m_size - str.length();
    
    for (std::size_t i = 0; i < search_size; ++i) {
        if (std::memcmp(search_data + i, str.data(), str.length()) == 0) {
            return result<c_address_internal>::ok(c_address_internal(m_base_address + i));
        }
    }
    
    return result<c_address_internal>::error("String not found");
}

inline result<std::vector<c_address_internal>> c_memory::find_all_strings(std::string_view str) const {
    if (!is_valid()) {
        return result<std::vector<c_address_internal>>::error("Invalid module");
    }
    
    if (str.empty()) {
        return result<std::vector<c_address_internal>>::error("Empty search string");
    }
    
    std::vector<c_address_internal> results;
    const auto* search_data = reinterpret_cast<const char*>(m_base_address);
    const auto search_size = m_size - str.length();
    
    for (std::size_t i = 0; i < search_size; ++i) {
        if (std::memcmp(search_data + i, str.data(), str.length()) == 0) {
            results.emplace_back(m_base_address + i);
        }
    }
    
    if (results.empty()) {
        return result<std::vector<c_address_internal>>::error("No strings found");
    }
    
    return result<std::vector<c_address_internal>>::ok(std::move(results));
}

inline result<c_address_internal> c_memory::get_export(std::string_view export_name) const {
    if (!is_valid()) {
        return result<c_address_internal>::error("Invalid module");
    }
    
    const auto proc_address = ::GetProcAddress(m_module_handle, export_name.data());
    if (!proc_address) {
        return result<c_address_internal>::error("Export not found: " + std::string(export_name));
    }
    
    return result<c_address_internal>::ok(c_address_internal(reinterpret_cast<std::uintptr_t>(proc_address)));
}

inline std::vector<std::uint8_t> c_memory::parse_pattern(std::string_view pattern) const {
    std::vector<std::uint8_t> result;
    
    for (std::size_t i = 0; i < pattern.length(); i += 3) {
        if (i + 1 >= pattern.length()) break;
        
        if (pattern[i] == '?' && pattern[i + 1] == '?') {
            result.push_back(0x00);  // Wildcard byte
        } else {
            const auto hex_str = pattern.substr(i, 2);
            try {
                const auto byte_val = static_cast<std::uint8_t>(std::stoul(std::string(hex_str), nullptr, 16));
                result.push_back(byte_val);
            } catch (...) {
                return {};  // Invalid pattern
            }
        }
    }
    
    return result;
}

inline std::vector<bool> c_memory::parse_mask(std::string_view pattern) const {
    std::vector<bool> result;
    
    for (std::size_t i = 0; i < pattern.length(); i += 3) {
        if (i + 1 >= pattern.length()) break;
        
        if (pattern[i] == '?' && pattern[i + 1] == '?') {
            result.push_back(false);  // Wildcard
        } else {
            result.push_back(true);   // Exact match
        }
    }
    
    return result;
}

inline bool c_memory::pattern_matches(std::uintptr_t address, 
                                     const std::vector<std::uint8_t>& pattern,
                                     const std::vector<bool>& mask) const {
    const auto* memory = reinterpret_cast<const std::uint8_t*>(address);
    
    for (std::size_t i = 0; i < pattern.size(); ++i) {
        if (mask[i] && memory[i] != pattern[i]) {
            return false;
        }
    }
    
    return true;
}

}  // namespace vision::utils::memory
