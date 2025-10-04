// XOR String Usage Examples - Demonstrates secure string obfuscation
// This file shows how to use the improved xorstr system for maximum security

#include <vision/utils/xorstr.hpp>
#include <vision/logger.hpp>
#include <iostream>

using namespace vision::utils;

void demonstrate_xorstr_usage() {
    // ========================================
    // 1. Basic Usage (Least Secure)
    // ========================================
    
    // Simple XOR string - decrypts to temporary std::string
    std::string basic = xorstr("Hello World");
    std::cout << "Basic: " << basic << std::endl;
    // WARNING: 'basic' string remains in memory until scope ends
    
    
    // ========================================
    // 2. Secure Usage (Recommended)
    // ========================================
    
    // Secure string - automatically clears memory on destruction
    {
        auto secure = xorstr_secure("Sensitive API Key");
        std::cout << "Secure: " << secure.c_str() << std::endl;
        // Memory is automatically cleared when 'secure' goes out of scope
    }
    
    
    // ========================================
    // 3. Stack-Based Usage (Most Secure)
    // ========================================
    
    // Stack-based decryption - string never exists in heap memory
    xorstr_call("Super Secret Password", [](const char* password) {
        // Use the password immediately
        std::cout << "Stack-based: " << password << std::endl;
        
        // You can call functions with it
        // some_api_function(password);
        
        // Buffer is automatically cleared after this lambda
    });
    
    
    // ========================================
    // 4. Real-World Examples
    // ========================================
    
    // Example 1: API Keys
    xorstr_call("sk-1234567890abcdef", [](const char* api_key) {
        // Use API key for authentication
        // http_client.set_auth_header(api_key);
        std::cout << "API Key used (not shown)" << std::endl;
    });
    
    // Example 2: Registry Keys
    xorstr_call("SOFTWARE\\Microsoft\\Windows\\CurrentVersion", [](const char* reg_path) {
        // Open registry key
        // HKEY key;
        // RegOpenKeyEx(HKEY_LOCAL_MACHINE, reg_path, 0, KEY_READ, &key);
        std::cout << "Registry path: " << reg_path << std::endl;
    });
    
    // Example 3: File Paths
    xorstr_call("C:\\Windows\\System32\\kernel32.dll", [](const char* dll_path) {
        // Load library
        // HMODULE module = LoadLibraryA(dll_path);
        std::cout << "DLL path: " << dll_path << std::endl;
    });
    
    // Example 4: Function Names for GetProcAddress
    xorstr_call("NtQueryInformationProcess", [](const char* func_name) {
        // Get function address
        // FARPROC proc = GetProcAddress(ntdll, func_name);
        std::cout << "Function name: " << func_name << std::endl;
    });
    
    
    // ========================================
    // 5. Logging with Secure Strings
    // ========================================
    
    // Initialize logger with secure name
    {
        auto logger_name = xorstr_secure("vision_component");
        // logger::initialize(logger_name.get(), logger::log_level::info);
        std::cout << "Logger initialized with: " << logger_name.c_str() << std::endl;
    }
    
    // Log messages with obfuscated strings
    xorstr_call("Initializing game hook system", [](const char* message) {
        // logger::log_info(message);
        std::cout << "Log: " << message << std::endl;
    });
    
    
    // ========================================
    // 6. Pattern Scanning with Obfuscated Patterns
    // ========================================
    
    // Obfuscate byte patterns
    xorstr_call("E8 ?? ?? ?? ?? 83 C4 08", [](const char* pattern) {
        // auto result = memory.find_pattern(pattern);
        std::cout << "Scanning for pattern: " << pattern << std::endl;
    });
    
    // Obfuscate search strings
    xorstr_call("Send Battle", [](const char* search_str) {
        // auto result = memory.find_string(search_str);
        std::cout << "Searching for string: " << search_str << std::endl;
    });
    
    
    // ========================================
    // 7. Multiple Strings in One Call
    // ========================================
    
    // You can use multiple XOR strings in the same function
    auto process_credentials = []() {
        xorstr_call("username", [](const char* user_field) {
            xorstr_call("admin123", [user_field](const char* username) {
                xorstr_call("password", [user_field, username](const char* pass_field) {
                    xorstr_call("secret456", [user_field, username, pass_field](const char* password) {
                        std::cout << "Credentials: " << user_field << "=" << username 
                                  << ", " << pass_field << "=" << password << std::endl;
                        // All strings are cleared automatically after this scope
                    });
                });
            });
        });
    };
    
    process_credentials();
    
    
    // ========================================
    // 8. Compile-Time vs Runtime Verification
    // ========================================
    
    std::cout << "\n=== Security Verification ===" << std::endl;
    
    // This string is encrypted at compile-time
    auto encrypted_data = xorstr("This string is encrypted at compile-time");
    std::cout << "Encrypted size: " << encrypted_data.size() << " bytes" << std::endl;
    
    // The actual string data in the binary is XOR-encrypted
    // Only when you call decrypt() or use xorstr_call() does it get decrypted
    
    // Demonstrate that each usage has a different key
    auto str1 = xorstr("same string");
    auto str2 = xorstr("same string");  // Different key due to different line number
    
    std::cout << "Same string, different encryption keys (different line numbers)" << std::endl;
    std::cout << "String 1: " << str1.decrypt() << std::endl;
    std::cout << "String 2: " << str2.decrypt() << std::endl;
}

// Example of integrating with existing Vision code
void vision_integration_example() {
    std::cout << "\n=== Vision Integration Examples ===" << std::endl;
    
    // Example: Secure process attachment
    xorstr_call("metin2client.exe", [](const char* process_name) {
        std::cout << "Attaching to process: " << process_name << std::endl;
        // auto process = c_process::get(process_name);
    });
    
    // Example: Secure DLL injection
    xorstr_call("vision_internal.dll", [](const char* dll_name) {
        std::cout << "Injecting DLL: " << dll_name << std::endl;
        // inject_dll(dll_name);
    });
    
    // Example: Secure window class search
    xorstr_call("METIN2_GAME", [](const char* window_class) {
        std::cout << "Finding window class: " << window_class << std::endl;
        // auto process = c_process::from_class(window_class);
    });
    
    // Example: Secure registry operations
    xorstr_call("SOFTWARE\\Vision\\Settings", [](const char* reg_key) {
        std::cout << "Registry key: " << reg_key << std::endl;
        // Read/write configuration from registry
    });
}

int main() {
    std::cout << "=== XOR String Security Demonstration ===" << std::endl;
    std::cout << "This demonstrates compile-time encryption with runtime decryption" << std::endl;
    std::cout << "Strings are encrypted in the binary and only decrypted when used" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    demonstrate_xorstr_usage();
    vision_integration_example();
    
    std::cout << "\n=== Security Benefits ===" << std::endl;
    std::cout << "✓ Strings are XOR-encrypted at compile-time" << std::endl;
    std::cout << "✓ Each usage has a unique encryption key" << std::endl;
    std::cout << "✓ Runtime decryption only when needed" << std::endl;
    std::cout << "✓ Automatic memory clearing after use" << std::endl;
    std::cout << "✓ Stack-based decryption avoids heap allocation" << std::endl;
    std::cout << "✓ Resistant to static analysis and memory dumps" << std::endl;
    
    return 0;
}
