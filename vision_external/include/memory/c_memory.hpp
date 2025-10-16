#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <optional>
#include <vision/common/logger.hpp>

namespace vision {
namespace memory {

class c_memory {
public:
    c_memory() : process_handle_(nullptr), process_id_(0) {}
    ~c_memory() {
        close_process();
    }

    // Process management
    bool attach_to_process(const std::string& process_name);
    bool attach_to_process(DWORD process_id);
    void close_process();
    bool is_attached() const { return process_handle_ != nullptr; }
    
    HANDLE get_process_handle() const { return process_handle_; }
    DWORD get_process_id() const { return process_id_; }

    // Memory reading
    template<typename T>
    std::optional<T> read(uintptr_t address) {
        T value{};
        if (read_memory(address, &value, sizeof(T))) {
            return value;
        }
        return std::nullopt;
    }

    bool read_memory(uintptr_t address, void* buffer, size_t size);
    std::vector<uint8_t> read_bytes(uintptr_t address, size_t size);
    std::string read_string(uintptr_t address, size_t max_length = 256);

    // Memory writing
    template<typename T>
    bool write(uintptr_t address, const T& value) {
        return write_memory(address, &value, sizeof(T));
    }

    bool write_memory(uintptr_t address, const void* data, size_t size);
    bool write_bytes(uintptr_t address, const std::vector<uint8_t>& bytes);
    bool write_string(uintptr_t address, const std::string& str);

    // Memory protection
    bool protect_memory(uintptr_t address, size_t size, DWORD new_protection, DWORD* old_protection = nullptr);
    bool unprotect_memory(uintptr_t address, size_t size);
    std::optional<MEMORY_BASIC_INFORMATION> query_memory(uintptr_t address);

    // Memory allocation
    uintptr_t allocate_memory(size_t size, DWORD allocation_type = MEM_COMMIT | MEM_RESERVE, 
                             DWORD protection = PAGE_EXECUTE_READWRITE);
    bool free_memory(uintptr_t address, size_t size = 0, DWORD free_type = MEM_RELEASE);

    // Pattern scanning
    uintptr_t pattern_scan(const std::string& pattern, const std::string& mask, 
                          uintptr_t start_address = 0, uintptr_t end_address = 0);
    std::vector<uintptr_t> pattern_scan_all(const std::string& pattern, const std::string& mask,
                                           uintptr_t start_address = 0, uintptr_t end_address = 0);

    // Module operations
    uintptr_t get_module_base(const std::string& module_name);
    size_t get_module_size(const std::string& module_name);
    std::vector<std::string> get_loaded_modules();

    // DLL injection
    bool inject_dll(const std::string& dll_path);
    bool eject_dll(const std::string& dll_name);

    // Utility functions
    bool is_valid_address(uintptr_t address);
    std::string get_last_error_string();

private:
    HANDLE process_handle_;
    DWORD process_id_;

    // Helper functions
    DWORD find_process_id(const std::string& process_name);
    bool pattern_compare(const uint8_t* data, const std::string& pattern, const std::string& mask);
};

} // namespace memory
} // namespace vision
