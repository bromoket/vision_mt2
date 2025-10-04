#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace vision::utils::fnv {

// FNV-1a constants
constexpr std::uint32_t fnv_offset_basis_32 = 0x811c9dc5;
constexpr std::uint32_t fnv_prime_32 = 0x01000193;

constexpr std::uint64_t fnv_offset_basis_64 = 0xcbf29ce484222325;
constexpr std::uint64_t fnv_prime_64 = 0x00000100000001b3;

// Compile-time FNV-1a hash (32-bit)
constexpr std::uint32_t hash_32(std::string_view str) noexcept {
    std::uint32_t hash = fnv_offset_basis_32;
    
    for (const char c : str) {
        hash ^= static_cast<std::uint32_t>(static_cast<std::uint8_t>(c));
        hash *= fnv_prime_32;
    }
    
    return hash;
}

// Compile-time FNV-1a hash (64-bit)
constexpr std::uint64_t hash_64(std::string_view str) noexcept {
    std::uint64_t hash = fnv_offset_basis_64;
    
    for (const char c : str) {
        hash ^= static_cast<std::uint64_t>(static_cast<std::uint8_t>(c));
        hash *= fnv_prime_64;
    }
    
    return hash;
}

// Default to 32-bit hash
constexpr std::uint32_t hash(std::string_view str) noexcept {
    return hash_32(str);
}

// Runtime FNV-1a hash (32-bit)
inline std::uint32_t hash_runtime_32(std::string_view str) noexcept {
    std::uint32_t hash = fnv_offset_basis_32;
    
    for (const char c : str) {
        hash ^= static_cast<std::uint32_t>(static_cast<std::uint8_t>(c));
        hash *= fnv_prime_32;
    }
    
    return hash;
}

// Runtime FNV-1a hash (64-bit)
inline std::uint64_t hash_runtime_64(std::string_view str) noexcept {
    std::uint64_t hash = fnv_offset_basis_64;
    
    for (const char c : str) {
        hash ^= static_cast<std::uint64_t>(static_cast<std::uint8_t>(c));
        hash *= fnv_prime_64;
    }
    
    return hash;
}

// Default runtime hash (32-bit)
inline std::uint32_t hash_runtime(std::string_view str) noexcept {
    return hash_runtime_32(str);
}

}  // namespace vision::utils::fnv

// Convenience macros
#define FNV_HASH(str) vision::utils::fnv::hash(str)
#define FNV_HASH_32(str) vision::utils::fnv::hash_32(str)
#define FNV_HASH_64(str) vision::utils::fnv::hash_64(str)
