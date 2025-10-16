#include <memory/c_memory.hpp>
#include <sstream>
#include <algorithm>

namespace vision {
namespace memory {

bool c_memory::attach_to_process(const std::string& process_name) {
    DWORD pid = find_process_id(process_name);
    if (pid == 0) {
        LOG_ERROR("Process '{}' not found", process_name);
        return false;
    }
    return attach_to_process(pid);
}

bool c_memory::attach_to_process(DWORD process_id) {
    close_process();
    
    process_handle_ = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    if (!process_handle_) {
        LOG_ERROR("Failed to open process {}: {}", process_id, get_last_error_string());
        return false;
    }
    
    process_id_ = process_id;
    LOG_INFO("Successfully attached to process {} (PID: {})", process_id, process_id_);
    return true;
}

void c_memory::close_process() {
    if (process_handle_) {
        CloseHandle(process_handle_);
        process_handle_ = nullptr;
        process_id_ = 0;
        LOG_DEBUG("Process handle closed");
    }
}

bool c_memory::read_memory(uintptr_t address, void* buffer, size_t size) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return false;
    }
    
    SIZE_T bytes_read = 0;
    if (!ReadProcessMemory(process_handle_, reinterpret_cast<LPCVOID>(address), 
                          buffer, size, &bytes_read) || bytes_read != size) {
        LOG_ERROR("Failed to read {} bytes from 0x{:X}: {}", size, address, get_last_error_string());
        return false;
    }
    
    return true;
}

std::vector<uint8_t> c_memory::read_bytes(uintptr_t address, size_t size) {
    std::vector<uint8_t> buffer(size);
    if (read_memory(address, buffer.data(), size)) {
        return buffer;
    }
    return {};
}

std::string c_memory::read_string(uintptr_t address, size_t max_length) {
    std::vector<char> buffer(max_length + 1, 0);
    if (read_memory(address, buffer.data(), max_length)) {
        return std::string(buffer.data());
    }
    return "";
}

bool c_memory::write_memory(uintptr_t address, const void* data, size_t size) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return false;
    }
    
    SIZE_T bytes_written = 0;
    if (!WriteProcessMemory(process_handle_, reinterpret_cast<LPVOID>(address), 
                           data, size, &bytes_written) || bytes_written != size) {
        LOG_ERROR("Failed to write {} bytes to 0x{:X}: {}", size, address, get_last_error_string());
        return false;
    }
    
    return true;
}

bool c_memory::write_bytes(uintptr_t address, const std::vector<uint8_t>& bytes) {
    return write_memory(address, bytes.data(), bytes.size());
}

bool c_memory::write_string(uintptr_t address, const std::string& str) {
    return write_memory(address, str.c_str(), str.length() + 1);
}

bool c_memory::protect_memory(uintptr_t address, size_t size, DWORD new_protection, DWORD* old_protection) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return false;
    }
    
    DWORD old_protect = 0;
    if (!VirtualProtectEx(process_handle_, reinterpret_cast<LPVOID>(address), 
                         size, new_protection, &old_protect)) {
        LOG_ERROR("Failed to change protection for 0x{:X}: {}", address, get_last_error_string());
        return false;
    }
    
    if (old_protection) {
        *old_protection = old_protect;
    }
    
    LOG_DEBUG("Changed protection for 0x{:X} (size: {}) from 0x{:X} to 0x{:X}", 
              address, size, old_protect, new_protection);
    return true;
}

bool c_memory::unprotect_memory(uintptr_t address, size_t size) {
    return protect_memory(address, size, PAGE_EXECUTE_READWRITE);
}

std::optional<MEMORY_BASIC_INFORMATION> c_memory::query_memory(uintptr_t address) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return std::nullopt;
    }
    
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQueryEx(process_handle_, reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
        return mbi;
    }
    
    return std::nullopt;
}

uintptr_t c_memory::allocate_memory(size_t size, DWORD allocation_type, DWORD protection) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return 0;
    }
    
    LPVOID address = VirtualAllocEx(process_handle_, nullptr, size, allocation_type, protection);
    if (!address) {
        LOG_ERROR("Failed to allocate {} bytes: {}", size, get_last_error_string());
        return 0;
    }
    
    LOG_DEBUG("Allocated {} bytes at 0x{:X}", size, reinterpret_cast<uintptr_t>(address));
    return reinterpret_cast<uintptr_t>(address);
}

bool c_memory::free_memory(uintptr_t address, size_t size, DWORD free_type) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return false;
    }
    
    if (!VirtualFreeEx(process_handle_, reinterpret_cast<LPVOID>(address), size, free_type)) {
        LOG_ERROR("Failed to free memory at 0x{:X}: {}", address, get_last_error_string());
        return false;
    }
    
    LOG_DEBUG("Freed memory at 0x{:X}", address);
    return true;
}

uintptr_t c_memory::pattern_scan(const std::string& pattern, const std::string& mask, 
                                uintptr_t start_address, uintptr_t end_address) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return 0;
    }
    
    if (start_address == 0) {
        start_address = get_module_base(""); // Main module
    }
    
    if (end_address == 0) {
        end_address = start_address + get_module_size("");
    }
    
    const size_t pattern_size = pattern.length();
    std::vector<uint8_t> buffer(4096);
    
    for (uintptr_t addr = start_address; addr < end_address - pattern_size; addr += buffer.size() - pattern_size) {
        size_t read_size = std::min(buffer.size(), static_cast<size_t>(end_address - addr));
        
        if (!read_memory(addr, buffer.data(), read_size)) {
            continue;
        }
        
        for (size_t i = 0; i <= read_size - pattern_size; ++i) {
            if (pattern_compare(buffer.data() + i, pattern, mask)) {
                return addr + i;
            }
        }
    }
    
    return 0;
}

std::vector<uintptr_t> c_memory::pattern_scan_all(const std::string& pattern, const std::string& mask,
                                                 uintptr_t start_address, uintptr_t end_address) {
    std::vector<uintptr_t> results;
    uintptr_t current = start_address;
    
    while (true) {
        uintptr_t found = pattern_scan(pattern, mask, current, end_address);
        if (found == 0) break;
        
        results.push_back(found);
        current = found + 1;
    }
    
    return results;
}

uintptr_t c_memory::get_module_base(const std::string& module_name) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return 0;
    }
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id_);
    if (snapshot == INVALID_HANDLE_VALUE) {
        LOG_ERROR("Failed to create module snapshot: {}", get_last_error_string());
        return 0;
    }
    
    MODULEENTRY32 module_entry{};
    module_entry.dwSize = sizeof(MODULEENTRY32);
    
    if (Module32First(snapshot, &module_entry)) {
        do {
            std::string current_module = module_entry.szModule;
            std::transform(current_module.begin(), current_module.end(), current_module.begin(), ::tolower);
            
            std::string target_module = module_name;
            std::transform(target_module.begin(), target_module.end(), target_module.begin(), ::tolower);
            
            if (module_name.empty() || current_module == target_module) {
                CloseHandle(snapshot);
                return reinterpret_cast<uintptr_t>(module_entry.modBaseAddr);
            }
        } while (Module32Next(snapshot, &module_entry));
    }
    
    CloseHandle(snapshot);
    return 0;
}

size_t c_memory::get_module_size(const std::string& module_name) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return 0;
    }
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id_);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    MODULEENTRY32 module_entry{};
    module_entry.dwSize = sizeof(MODULEENTRY32);
    
    if (Module32First(snapshot, &module_entry)) {
        do {
            std::string current_module = module_entry.szModule;
            std::transform(current_module.begin(), current_module.end(), current_module.begin(), ::tolower);
            
            std::string target_module = module_name;
            std::transform(target_module.begin(), target_module.end(), target_module.begin(), ::tolower);
            
            if (module_name.empty() || current_module == target_module) {
                CloseHandle(snapshot);
                return module_entry.modBaseSize;
            }
        } while (Module32Next(snapshot, &module_entry));
    }
    
    CloseHandle(snapshot);
    return 0;
}

std::vector<std::string> c_memory::get_loaded_modules() {
    std::vector<std::string> modules;
    
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return modules;
    }
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id_);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return modules;
    }
    
    MODULEENTRY32 module_entry{};
    module_entry.dwSize = sizeof(MODULEENTRY32);
    
    if (Module32First(snapshot, &module_entry)) {
        do {
            modules.push_back(module_entry.szModule);
        } while (Module32Next(snapshot, &module_entry));
    }
    
    CloseHandle(snapshot);
    return modules;
}

bool c_memory::inject_dll(const std::string& dll_path) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return false;
    }
    
    // Allocate memory for DLL path
    size_t path_size = dll_path.length() + 1;
    uintptr_t remote_string = allocate_memory(path_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote_string) {
        return false;
    }
    
    // Write DLL path to remote process
    if (!write_string(remote_string, dll_path)) {
        free_memory(remote_string);
        return false;
    }
    
    // Get LoadLibraryA address
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    FARPROC load_library = GetProcAddress(kernel32, "LoadLibraryA");
    
    if (!load_library) {
        LOG_ERROR("Failed to get LoadLibraryA address");
        free_memory(remote_string);
        return false;
    }
    
    // Create remote thread
    HANDLE thread = CreateRemoteThread(process_handle_, nullptr, 0, 
                                      reinterpret_cast<LPTHREAD_START_ROUTINE>(load_library),
                                      reinterpret_cast<LPVOID>(remote_string), 0, nullptr);
    
    if (!thread) {
        LOG_ERROR("Failed to create remote thread: {}", get_last_error_string());
        free_memory(remote_string);
        return false;
    }
    
    // Wait for injection to complete
    WaitForSingleObject(thread, INFINITE);
    
    DWORD exit_code = 0;
    GetExitCodeThread(thread, &exit_code);
    CloseHandle(thread);
    
    free_memory(remote_string);
    
    if (exit_code == 0) {
        LOG_ERROR("DLL injection failed - LoadLibraryA returned NULL");
        return false;
    }
    
    LOG_INFO("Successfully injected DLL: {}", dll_path);
    return true;
}

bool c_memory::eject_dll(const std::string& dll_name) {
    if (!is_attached()) {
        LOG_ERROR("Not attached to any process");
        return false;
    }
    
    uintptr_t module_base = get_module_base(dll_name);
    if (!module_base) {
        LOG_ERROR("Module '{}' not found in target process", dll_name);
        return false;
    }
    
    // Get FreeLibrary address
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    FARPROC free_library = GetProcAddress(kernel32, "FreeLibrary");
    
    if (!free_library) {
        LOG_ERROR("Failed to get FreeLibrary address");
        return false;
    }
    
    // Create remote thread to unload DLL
    HANDLE thread = CreateRemoteThread(process_handle_, nullptr, 0,
                                      reinterpret_cast<LPTHREAD_START_ROUTINE>(free_library),
                                      reinterpret_cast<LPVOID>(module_base), 0, nullptr);
    
    if (!thread) {
        LOG_ERROR("Failed to create remote thread for DLL ejection: {}", get_last_error_string());
        return false;
    }
    
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    
    LOG_INFO("Successfully ejected DLL: {}", dll_name);
    return true;
}

bool c_memory::is_valid_address(uintptr_t address) {
    auto mbi = query_memory(address);
    return mbi.has_value() && (mbi->State == MEM_COMMIT);
}

std::string c_memory::get_last_error_string() {
    DWORD error = GetLastError();
    if (error == 0) return "No error";
    
    LPSTR message_buffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                                reinterpret_cast<LPSTR>(&message_buffer), 0, nullptr);
    
    std::string message(message_buffer, size);
    LocalFree(message_buffer);
    
    return message;
}

DWORD c_memory::find_process_id(const std::string& process_name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    PROCESSENTRY32 process_entry{};
    process_entry.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(snapshot, &process_entry)) {
        do {
            std::string current_process = process_entry.szExeFile;
            std::transform(current_process.begin(), current_process.end(), current_process.begin(), ::tolower);
            
            std::string target_process = process_name;
            std::transform(target_process.begin(), target_process.end(), target_process.begin(), ::tolower);
            
            if (current_process == target_process) {
                CloseHandle(snapshot);
                return process_entry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &process_entry));
    }
    
    CloseHandle(snapshot);
    return 0;
}

bool c_memory::pattern_compare(const uint8_t* data, const std::string& pattern, const std::string& mask) {
    for (size_t i = 0; i < pattern.length() && i < mask.length(); ++i) {
        if (mask[i] == 'x' && data[i] != static_cast<uint8_t>(pattern[i])) {
            return false;
        }
    }
    return true;
}

} // namespace memory
} // namespace vision
