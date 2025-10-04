#pragma once

#include <vision/result.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>

#ifdef WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

namespace vision::utils::memory {

// Forward declarations
class internal_memory_policy;
class external_memory_policy;

// Memory policies for different access methods
class internal_memory_policy {
public:
    template <typename T>
    static result<T> read(std::uintptr_t address) {
        if (!is_valid_address(address)) {
            return result<T>::error("Invalid memory address");
        }
        
        try {
            return result<T>::ok(*reinterpret_cast<T*>(address));
        } catch (...) {
            return result<T>::error("Failed to read memory");
        }
    }
    
    template <typename T>
    static result<void> write(std::uintptr_t address, const T& value) {
        if (!is_valid_address(address)) {
            return result<void>::error("Invalid memory address");
        }
        
        try {
            *reinterpret_cast<T*>(address) = value;
            return result<void>::ok();
        } catch (...) {
            return result<void>::error("Failed to write memory");
        }
    }
    
private:
    static bool is_valid_address(std::uintptr_t address) {
        return address != 0 && !::IsBadReadPtr(reinterpret_cast<void*>(address), sizeof(std::uintptr_t));
    }
};

class external_memory_policy {
public:
    explicit external_memory_policy(HANDLE process_handle) : m_process_handle(process_handle) {}
    
    template <typename T>
    result<T> read(std::uintptr_t address) const {
        if (!is_valid_address(address)) {
            return result<T>::error("Invalid memory address");
        }
        
        T value{};
        SIZE_T bytes_read = 0;
        
        if (!::ReadProcessMemory(m_process_handle, reinterpret_cast<LPCVOID>(address), 
                                &value, sizeof(T), &bytes_read)) {
            return result<T>::error("ReadProcessMemory failed");
        }
        
        if (bytes_read != sizeof(T)) {
            return result<T>::error("Incomplete memory read");
        }
        
        return result<T>::ok(value);
    }
    
    template <typename T>
    result<void> write(std::uintptr_t address, const T& value) const {
        if (!is_valid_address(address)) {
            return result<void>::error("Invalid memory address");
        }
        
        SIZE_T bytes_written = 0;
        
        if (!::WriteProcessMemory(m_process_handle, reinterpret_cast<LPVOID>(address), 
                                 &value, sizeof(T), &bytes_written)) {
            return result<void>::error("WriteProcessMemory failed");
        }
        
        if (bytes_written != sizeof(T)) {
            return result<void>::error("Incomplete memory write");
        }
        
        return result<void>::ok();
    }
    
private:
    bool is_valid_address(std::uintptr_t address) const {
        return address != 0 && m_process_handle != nullptr && m_process_handle != INVALID_HANDLE_VALUE;
    }
    
    HANDLE m_process_handle;
};

// Address wrapper template
template <typename Policy>
class c_address {
public:
    c_address() = default;
    
    explicit c_address(std::uintptr_t address) : m_address(address) {}
    
    template <typename T = Policy>
    explicit c_address(std::uintptr_t address, T&& policy) 
        : m_address(address), m_policy(std::forward<T>(policy)) {}
    
    // Address manipulation
    [[nodiscard]] c_address offset(std::ptrdiff_t offset) const {
        if constexpr (std::is_same_v<Policy, external_memory_policy>) {
            return c_address(m_address + offset, m_policy);
        } else {
            return c_address(m_address + offset);
        }
    }
    
    [[nodiscard]] result<c_address> relative(std::ptrdiff_t displacement, std::size_t instruction_size) const {
        // Read the displacement value
        auto disp_result = get<std::int32_t>();
        if (!disp_result) {
            return result<c_address>::error("Failed to read displacement: " + disp_result.message());
        }
        
        const auto target_address = m_address + instruction_size + disp_result.value();
        
        if constexpr (std::is_same_v<Policy, external_memory_policy>) {
            return result<c_address>::ok(c_address(target_address, m_policy));
        } else {
            return result<c_address>::ok(c_address(target_address));
        }
    }
    
    // Memory access
    template <typename T>
    [[nodiscard]] result<T> get() const {
        if constexpr (std::is_same_v<Policy, internal_memory_policy>) {
            return Policy::template read<T>(m_address);
        } else {
            return m_policy.template read<T>(m_address);
        }
    }
    
    template <typename T>
    result<void> set(const T& value) const {
        if constexpr (std::is_same_v<Policy, internal_memory_policy>) {
            return Policy::template write<T>(m_address, value);
        } else {
            return m_policy.template write<T>(m_address, value);
        }
    }
    
    // Direct pointer access (internal only)
    template <typename T>
    [[nodiscard]] T* as() const {
        static_assert(std::is_same_v<Policy, internal_memory_policy>, 
                     "Direct pointer access only available for internal memory policy");
        return reinterpret_cast<T*>(m_address);
    }
    
    // Validation
    [[nodiscard]] bool is_valid() const {
        return m_address != 0;
    }
    
    // Raw address access
    [[nodiscard]] std::uintptr_t raw() const {
        return m_address;
    }
    
    // Comparison operators
    [[nodiscard]] bool operator==(const c_address& other) const {
        return m_address == other.m_address;
    }
    
    [[nodiscard]] bool operator!=(const c_address& other) const {
        return !(*this == other);
    }
    
    [[nodiscard]] bool operator<(const c_address& other) const {
        return m_address < other.m_address;
    }
    
    // Implicit conversion to bool
    [[nodiscard]] explicit operator bool() const {
        return is_valid();
    }

private:
    std::uintptr_t m_address = 0;
    
    [[no_unique_address]] Policy m_policy{};
};

// Type aliases
using c_address_internal = c_address<internal_memory_policy>;
using c_address_external = c_address<external_memory_policy>;

}  // namespace vision::utils::memory
