#pragma once

#include <optional>
#include <string>
#include <type_traits>

namespace vision {

template <typename T>
class result {
public:
    // Constructors
    result() = delete;
    
    // Success constructor
    template <typename U = T>
    static result ok(U&& value) {
        return result(std::forward<U>(value));
    }
    
    // Error constructor
    static result error(std::string message) {
        return result(std::move(message));
    }
    
    // Copy constructor
    result(const result& other) = default;
    
    // Move constructor
    result(result&& other) noexcept = default;
    
    // Copy assignment
    result& operator=(const result& other) = default;
    
    // Move assignment
    result& operator=(result&& other) noexcept = default;
    
    // Destructor
    ~result() = default;
    
    // Check if result contains a value
    [[nodiscard]] bool has_value() const noexcept {
        return m_value.has_value();
    }
    
    // Implicit bool conversion
    [[nodiscard]] explicit operator bool() const noexcept {
        return has_value();
    }
    
    // Get the value (throws if error)
    [[nodiscard]] const T& value() const& {
        if (!has_value()) {
            throw std::runtime_error("Attempted to get value from error result: " + m_error_message);
        }
        return *m_value;
    }
    
    [[nodiscard]] T& value() & {
        if (!has_value()) {
            throw std::runtime_error("Attempted to get value from error result: " + m_error_message);
        }
        return *m_value;
    }
    
    [[nodiscard]] T&& value() && {
        if (!has_value()) {
            throw std::runtime_error("Attempted to get value from error result: " + m_error_message);
        }
        return std::move(*m_value);
    }
    
    // Get value or default
    template <typename U>
    [[nodiscard]] T value_or(U&& default_value) const& {
        return has_value() ? *m_value : static_cast<T>(std::forward<U>(default_value));
    }
    
    template <typename U>
    [[nodiscard]] T value_or(U&& default_value) && {
        return has_value() ? std::move(*m_value) : static_cast<T>(std::forward<U>(default_value));
    }
    
    // Get error message
    [[nodiscard]] const std::string& message() const noexcept {
        return m_error_message;
    }
    
    // Dereference operators
    [[nodiscard]] const T& operator*() const& {
        return value();
    }
    
    [[nodiscard]] T& operator*() & {
        return value();
    }
    
    [[nodiscard]] T&& operator*() && {
        return std::move(value());
    }
    
    // Arrow operator
    [[nodiscard]] const T* operator->() const {
        return &value();
    }
    
    [[nodiscard]] T* operator->() {
        return &value();
    }

private:
    // Success constructor
    template <typename U>
    explicit result(U&& value) : m_value(std::forward<U>(value)) {}
    
    // Error constructor
    explicit result(std::string error_message) : m_error_message(std::move(error_message)) {}
    
    std::optional<T> m_value;
    std::string m_error_message;
};

// Specialization for void
template <>
class result<void> {
public:
    // Constructors
    result() = delete;
    
    // Success constructor
    static result ok() {
        return result(true);
    }
    
    // Error constructor
    static result error(std::string message) {
        return result(std::move(message));
    }
    
    // Copy constructor
    result(const result& other) = default;
    
    // Move constructor
    result(result&& other) noexcept = default;
    
    // Copy assignment
    result& operator=(const result& other) = default;
    
    // Move assignment
    result& operator=(result&& other) noexcept = default;
    
    // Destructor
    ~result() = default;
    
    // Check if result is success
    [[nodiscard]] bool has_value() const noexcept {
        return m_success;
    }
    
    // Implicit bool conversion
    [[nodiscard]] explicit operator bool() const noexcept {
        return has_value();
    }
    
    // Get error message
    [[nodiscard]] const std::string& message() const noexcept {
        return m_error_message;
    }

private:
    // Success constructor
    explicit result(bool success) : m_success(success) {}
    
    // Error constructor
    explicit result(std::string error_message) 
        : m_success(false), m_error_message(std::move(error_message)) {}
    
    bool m_success = false;
    std::string m_error_message;
};

}  // namespace vision
