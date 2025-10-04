#pragma once

#include <array>
#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>

namespace vision::utils {

// Compile-time random number generator for better key generation
constexpr std::uint32_t fnv1a_32(const char* str, std::size_t len) {
    std::uint32_t hash = 0x811c9dc5;
    for (std::size_t i = 0; i < len; ++i) {
        hash ^= static_cast<std::uint32_t>(str[i]);
        hash *= 0x01000193;
    }
    return hash;
}

// Generate compile-time key based on file, line, and time
#define XORSTR_KEY() (fnv1a_32(__FILE__ __TIME__, sizeof(__FILE__ __TIME__) - 1) + __LINE__)

template <std::size_t N, std::uint32_t Key>
class xor_string {
public:
    // Compile-time encryption
    constexpr xor_string(const char (&str)[N]) : m_encrypted{} {
        for (std::size_t i = 0; i < N; ++i) {
            m_encrypted[i] = str[i] ^ static_cast<char>((Key + i) & 0xFF);
        }
    }

    // Runtime decryption - returns temporary string that gets destroyed
    std::string decrypt() const {
        std::string result;
        result.reserve(N - 1);  // -1 for null terminator
        
        for (std::size_t i = 0; i < N - 1; ++i) {
            result += static_cast<char>(m_encrypted[i] ^ static_cast<char>((Key + i) & 0xFF));
        }
        
        return result;
    }

    // Stack-based decryption for temporary use (more secure)
    template <typename Func>
    void decrypt_call(Func&& func) const {
        char buffer[N];
        
        // Decrypt to stack buffer
        for (std::size_t i = 0; i < N - 1; ++i) {
            buffer[i] = m_encrypted[i] ^ static_cast<char>((Key + i) & 0xFF);
        }
        buffer[N - 1] = '\0';
        
        // Call function with decrypted string
        func(buffer);
        
        // Clear buffer (anti-memory dump)
        volatile char* volatile_buffer = buffer;
        for (std::size_t i = 0; i < N; ++i) {
            volatile_buffer[i] = 0;
        }
    }

    // Secure c_str() - use with caution, prefer decrypt_call()
    const char* c_str() const {
        thread_local char buffer[N];
        
        // Decrypt to thread-local buffer
        for (std::size_t i = 0; i < N - 1; ++i) {
            buffer[i] = m_encrypted[i] ^ static_cast<char>((Key + i) & 0xFF);
        }
        buffer[N - 1] = '\0';
        
        return buffer;
    }

    // Implicit conversion - creates temporary string
    operator std::string() const {
        return decrypt();
    }

    // Get encrypted size
    constexpr std::size_t size() const { return N - 1; }
    constexpr std::size_t length() const { return N - 1; }

private:
    std::array<char, N> m_encrypted;
};

// Deduction guide
template <std::size_t N, std::uint32_t Key>
xor_string(const char (&)[N]) -> xor_string<N, Key>;

// Secure string wrapper for automatic cleanup
class secure_string {
public:
    explicit secure_string(std::string&& str) : m_data(std::move(str)) {}
    
    ~secure_string() {
        // Clear memory on destruction
        if (!m_data.empty()) {
            volatile char* volatile_data = const_cast<char*>(m_data.data());
            for (std::size_t i = 0; i < m_data.size(); ++i) {
                volatile_data[i] = 0;
            }
        }
    }
    
    // Non-copyable, movable
    secure_string(const secure_string&) = delete;
    secure_string& operator=(const secure_string&) = delete;
    secure_string(secure_string&&) = default;
    secure_string& operator=(secure_string&&) = default;
    
    const std::string& get() const { return m_data; }
    const char* c_str() const { return m_data.c_str(); }
    std::size_t size() const { return m_data.size(); }
    bool empty() const { return m_data.empty(); }
    
private:
    std::string m_data;
};

}  // namespace vision::utils

// Main macro - creates XOR string with unique key per usage
#define xorstr(str) vision::utils::xor_string<sizeof(str), XORSTR_KEY()>(str)

// Secure usage macro - automatically cleans up after use
#define xorstr_secure(str) vision::utils::secure_string(xorstr(str).decrypt())

// Stack-based secure usage - for temporary operations
#define xorstr_call(str, func) xorstr(str).decrypt_call(func)
