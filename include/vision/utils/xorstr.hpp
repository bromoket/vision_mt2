#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace vision::utils {

template <std::size_t N>
class xor_string {
public:
    constexpr xor_string(const char (&str)[N]) : m_encrypted{} {
        for (std::size_t i = 0; i < N; ++i) {
            m_encrypted[i] = str[i] ^ (key + i);
        }
    }

    std::string decrypt() const {
        std::string result;
        result.reserve(N - 1);  // -1 for null terminator
        
        for (std::size_t i = 0; i < N - 1; ++i) {
            result += static_cast<char>(m_encrypted[i] ^ (key + i));
        }
        
        return result;
    }

    const char* c_str() const {
        thread_local std::string decrypted;
        decrypted = decrypt();
        return decrypted.c_str();
    }

    operator std::string() const {
        return decrypt();
    }

private:
    static constexpr char key = __TIME__[7] + __TIME__[6] + __TIME__[4] + __TIME__[3] + __TIME__[1] + __TIME__[0];
    std::array<char, N> m_encrypted;
};

// Deduction guide
template <std::size_t N>
xor_string(const char (&)[N]) -> xor_string<N>;

}  // namespace vision::utils

// Convenience macro
#define xorstr(str) vision::utils::xor_string(str)
