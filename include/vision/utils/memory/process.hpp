#pragma once

#include <vision/result.hpp>
#include <vision/utils/memory/address.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

#ifdef WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#undef WIN32_LEAN_AND_MEAN
#endif

namespace vision::utils::memory {

class c_process {
public:
    // Constructors
    c_process() = default;
    explicit c_process(HANDLE process_handle, DWORD process_id);
    
    // Non-copyable, movable
    c_process(const c_process&) = delete;
    c_process& operator=(const c_process&) = delete;
    c_process(c_process&& other) noexcept;
    c_process& operator=(c_process&& other) noexcept;
    
    // Destructor
    ~c_process();
    
    // Static factory methods
    [[nodiscard]] static result<void> enable_debug_privileges();
    [[nodiscard]] static result<c_process> get(std::string_view process_name);
    [[nodiscard]] static result<c_process> from_pid(DWORD process_id);
    [[nodiscard]] static result<c_process> from_window(std::string_view window_title);
    [[nodiscard]] static result<c_process> from_class(std::string_view window_class);
    
    // Pattern scanning
    [[nodiscard]] result<c_address_external> find_pattern(std::string_view pattern) const;
    [[nodiscard]] result<std::vector<c_address_external>> find_all_patterns(std::string_view pattern) const;
    
    // String searching
    [[nodiscard]] result<c_address_external> find_string(std::string_view str) const;
    [[nodiscard]] result<std::vector<c_address_external>> find_all_strings(std::string_view str) const;
    
    // Export resolution
    [[nodiscard]] result<c_address_external> get_export(std::string_view module_name, std::string_view export_name) const;
    
    // Module information
    [[nodiscard]] result<std::uintptr_t> get_module_base(std::string_view module_name) const;
    [[nodiscard]] result<std::size_t> get_module_size(std::string_view module_name) const;
    
    // Process information
    [[nodiscard]] HANDLE handle() const { return m_process_handle; }
    [[nodiscard]] DWORD id() const { return m_process_id; }
    [[nodiscard]] bool is_valid() const { return m_process_handle != nullptr && m_process_handle != INVALID_HANDLE_VALUE; }
    
    // Memory operations
    [[nodiscard]] result<std::vector<std::uint8_t>> read_bytes(std::uintptr_t address, std::size_t size) const;
    [[nodiscard]] result<void> write_bytes(std::uintptr_t address, const std::vector<std::uint8_t>& data) const;

private:
    // Pattern parsing and matching
    [[nodiscard]] std::vector<std::uint8_t> parse_pattern(std::string_view pattern) const;
    [[nodiscard]] std::vector<bool> parse_mask(std::string_view pattern) const;
    [[nodiscard]] bool pattern_matches(const std::vector<std::uint8_t>& memory, std::size_t offset,
                                      const std::vector<std::uint8_t>& pattern,
                                      const std::vector<bool>& mask) const;
    
    // Memory region enumeration
    [[nodiscard]] std::vector<MEMORY_BASIC_INFORMATION> get_memory_regions() const;
    
    HANDLE m_process_handle = nullptr;
    DWORD m_process_id = 0;
};

// Implementation details
inline c_process::c_process(HANDLE process_handle, DWORD process_id) 
    : m_process_handle(process_handle), m_process_id(process_id) {}

inline c_process::c_process(c_process&& other) noexcept 
    : m_process_handle(other.m_process_handle), m_process_id(other.m_process_id) {
    other.m_process_handle = nullptr;
    other.m_process_id = 0;
}

inline c_process& c_process::operator=(c_process&& other) noexcept {
    if (this != &other) {
        if (m_process_handle && m_process_handle != INVALID_HANDLE_VALUE) {
            ::CloseHandle(m_process_handle);
        }
        
        m_process_handle = other.m_process_handle;
        m_process_id = other.m_process_id;
        
        other.m_process_handle = nullptr;
        other.m_process_id = 0;
    }
    return *this;
}

inline c_process::~c_process() {
    if (m_process_handle && m_process_handle != INVALID_HANDLE_VALUE) {
        ::CloseHandle(m_process_handle);
    }
}

inline result<void> c_process::enable_debug_privileges() {
    HANDLE token_handle = nullptr;
    
    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token_handle)) {
        return result<void>::error("Failed to open process token");
    }
    
    TOKEN_PRIVILEGES token_privileges{};
    token_privileges.PrivilegeCount = 1;
    token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    
    if (!::LookupPrivilegeValueA(nullptr, SE_DEBUG_NAME, &token_privileges.Privileges[0].Luid)) {
        ::CloseHandle(token_handle);
        return result<void>::error("Failed to lookup debug privilege");
    }
    
    const auto success = ::AdjustTokenPrivileges(token_handle, FALSE, &token_privileges, 
                                                sizeof(token_privileges), nullptr, nullptr);
    ::CloseHandle(token_handle);
    
    if (!success) {
        return result<void>::error("Failed to adjust token privileges");
    }
    
    return result<void>::ok();
}

inline result<c_process> c_process::get(std::string_view process_name) {
    const auto snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return result<c_process>::error("Failed to create process snapshot");
    }
    
    PROCESSENTRY32A process_entry{};
    process_entry.dwSize = sizeof(process_entry);
    
    if (!::Process32FirstA(snapshot, &process_entry)) {
        ::CloseHandle(snapshot);
        return result<c_process>::error("Failed to enumerate processes");
    }
    
    do {
        if (process_name == process_entry.szExeFile) {
            ::CloseHandle(snapshot);
            return from_pid(process_entry.th32ProcessID);
        }
    } while (::Process32NextA(snapshot, &process_entry));
    
    ::CloseHandle(snapshot);
    return result<c_process>::error("Process not found: " + std::string(process_name));
}

inline result<c_process> c_process::from_pid(DWORD process_id) {
    const auto process_handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    if (!process_handle) {
        return result<c_process>::error("Failed to open process with PID: " + std::to_string(process_id));
    }
    
    return result<c_process>::ok(c_process(process_handle, process_id));
}

inline result<c_process> c_process::from_window(std::string_view window_title) {
    const auto window_handle = ::FindWindowA(nullptr, window_title.data());
    if (!window_handle) {
        return result<c_process>::error("Window not found: " + std::string(window_title));
    }
    
    DWORD process_id = 0;
    ::GetWindowThreadProcessId(window_handle, &process_id);
    
    if (process_id == 0) {
        return result<c_process>::error("Failed to get process ID from window");
    }
    
    return from_pid(process_id);
}

inline result<c_process> c_process::from_class(std::string_view window_class) {
    const auto window_handle = ::FindWindowA(window_class.data(), nullptr);
    if (!window_handle) {
        return result<c_process>::error("Window class not found: " + std::string(window_class));
    }
    
    DWORD process_id = 0;
    ::GetWindowThreadProcessId(window_handle, &process_id);
    
    if (process_id == 0) {
        return result<c_process>::error("Failed to get process ID from window class");
    }
    
    return from_pid(process_id);
}

inline result<c_address_external> c_process::find_pattern(std::string_view pattern) const {
    if (!is_valid()) {
        return result<c_address_external>::error("Invalid process handle");
    }
    
    const auto parsed_pattern = parse_pattern(pattern);
    const auto mask = parse_mask(pattern);
    
    if (parsed_pattern.empty()) {
        return result<c_address_external>::error("Invalid pattern format");
    }
    
    const auto memory_regions = get_memory_regions();
    
    for (const auto& region : memory_regions) {
        if (region.State != MEM_COMMIT || region.Protect & PAGE_NOACCESS) {
            continue;
        }
        
        const auto memory_result = read_bytes(reinterpret_cast<std::uintptr_t>(region.BaseAddress), region.RegionSize);
        if (!memory_result) {
            continue;
        }
        
        const auto& memory = memory_result.value();
        
        for (std::size_t i = 0; i <= memory.size() - parsed_pattern.size(); ++i) {
            if (pattern_matches(memory, i, parsed_pattern, mask)) {
                const auto address = reinterpret_cast<std::uintptr_t>(region.BaseAddress) + i;
                return result<c_address_external>::ok(c_address_external(address, external_memory_policy(m_process_handle)));
            }
        }
    }
    
    return result<c_address_external>::error("Pattern not found");
}

inline result<std::vector<std::uint8_t>> c_process::read_bytes(std::uintptr_t address, std::size_t size) const {
    if (!is_valid()) {
        return result<std::vector<std::uint8_t>>::error("Invalid process handle");
    }
    
    std::vector<std::uint8_t> buffer(size);
    SIZE_T bytes_read = 0;
    
    if (!::ReadProcessMemory(m_process_handle, reinterpret_cast<LPCVOID>(address), 
                            buffer.data(), size, &bytes_read)) {
        return result<std::vector<std::uint8_t>>::error("ReadProcessMemory failed");
    }
    
    if (bytes_read != size) {
        buffer.resize(bytes_read);
    }
    
    return result<std::vector<std::uint8_t>>::ok(std::move(buffer));
}

inline std::vector<std::uint8_t> c_process::parse_pattern(std::string_view pattern) const {
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

inline std::vector<bool> c_process::parse_mask(std::string_view pattern) const {
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

inline bool c_process::pattern_matches(const std::vector<std::uint8_t>& memory, std::size_t offset,
                                      const std::vector<std::uint8_t>& pattern,
                                      const std::vector<bool>& mask) const {
    for (std::size_t i = 0; i < pattern.size(); ++i) {
        if (mask[i] && memory[offset + i] != pattern[i]) {
            return false;
        }
    }
    
    return true;
}

inline std::vector<MEMORY_BASIC_INFORMATION> c_process::get_memory_regions() const {
    std::vector<MEMORY_BASIC_INFORMATION> regions;
    
    std::uintptr_t address = 0;
    MEMORY_BASIC_INFORMATION mbi{};
    
    while (::VirtualQueryEx(m_process_handle, reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
        regions.push_back(mbi);
        address += mbi.RegionSize;
    }
    
    return regions;
}

}  // namespace vision::utils::memory
