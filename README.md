# Vision - Metin2 Game Helper Tool

A modern C++20 game helper tool for Metin2, featuring both internal (DLL injection) and external (process manipulation) components.

## 🏗️ Architecture

The project consists of two main components:

- **vision_internal**: DLL injected into `metin2client.exe` for direct memory access
- **vision_external**: Standalone executable for external process manipulation and UI

## 🔧 Requirements

### Build Tools
- **Clang++** compiler (required for 32-bit Windows builds)
- **CMake** 3.21 or higher
- **vcpkg** package manager
- **Visual Studio** (optional, for IDE support)

### Dependencies (managed by vcpkg)
- **fmt** - Modern formatting library
- **nlohmann-json** - JSON parsing/serialization
- **zydis** - Disassembly library for pattern analysis

### External Dependencies
- **ImGui** - Copy your ImGui installation to `external/imgui/`

## 🚀 Building

### 1. Setup vcpkg
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

### 2. Set Environment Variable
```bash
set VCPKG_ROOT=C:\path\to\vcpkg
```

### 3. Install Dependencies
```bash
vcpkg install fmt:x86-windows nlohmann-json:x86-windows zydis:x86-windows
```

### 4. Copy ImGui
Copy your ImGui source files to `external/imgui/`:
```
external/imgui/
├── imgui.cpp
├── imgui.h
├── imgui_demo.cpp
├── imgui_draw.cpp
├── imgui_tables.cpp
├── imgui_widgets.cpp
└── backends/
    ├── imgui_impl_win32.cpp
    ├── imgui_impl_win32.h
    ├── imgui_impl_dx11.cpp
    └── imgui_impl_dx11.h
```

### 5. Build the Project

#### Option A: Using the Build Scripts (Recommended)

**Windows Batch Script:**
```cmd
build.bat
```

**PowerShell Script (Advanced):**
```powershell
# Basic build
.\build.ps1

# Debug build
.\build.ps1 -Config Debug

# Clean build
.\build.ps1 -Clean

# Help
.\build.ps1 -Help
```

#### Option B: Manual CMake Build
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A Win32
cmake --build . --config Release
```

## 🔧 Git Setup

### Option 1: Complete GitHub Setup (Recommended)
Creates repository on GitHub and sets up everything:

```powershell
# Install Git Credential Manager and create GitHub repo
.\setup-github.ps1

# Create private repository
.\setup-github.ps1 -Private

# Custom repository name
.\setup-github.ps1 -RepoName "my-vision-fork"
```

### Option 2: Manual Git Setup
For local setup or other Git providers:

```powershell
# Install Git Credential Manager only
.\install-gcm.ps1

# Then configure Git
.\setup-git.ps1 -UserName "YourName" -UserEmail "your.email@example.com"
```

**Features provided:**
- ✅ **Git Credential Manager** - Secure token storage, no more passwords!
- ✅ **GitHub CLI** - Create repositories from command line
- ✅ **SSH Key Generation** - For maximum security
- ✅ **Automatic Authentication** - One-time setup, permanent convenience
- ✅ **Repository Creation** - Creates and pushes to GitHub automatically

## 📁 Project Structure

```
vision/
├── CMakeLists.txt              # Main CMake configuration
├── vcpkg.json                  # Package dependencies
├── build.bat                   # Windows batch build script
├── build.ps1                   # PowerShell build script
├── setup-git.ps1               # Git configuration script
├── setup-github.ps1             # Complete GitHub setup script
├── install-gcm.ps1             # Git Credential Manager installer
├── config.example.json         # Sample configuration file
├── xorstr_examples.cpp         # XOR string usage examples
├── .gitignore                  # Git ignore rules
├── .clang-format              # Code formatting rules
├── .clangd                    # LSP configuration
├── .editorconfig              # Editor settings
├── external/
│   └── imgui/                 # ImGui source (user-provided)
├── include/
│   └── vision/
│       ├── config.hpp         # Build configuration
│       ├── logger.hpp         # Logging system
│       ├── result.hpp         # Error handling
│       ├── utils/
│       │   ├── xorstr.hpp     # String obfuscation
│       │   ├── fnv.hpp        # Hash functions
│       │   ├── math_utils.hpp # Math utilities
│       │   └── memory/
│       │       ├── address.hpp    # Memory address wrapper
│       │       ├── memory.hpp     # Internal memory access
│       │       └── process.hpp    # External process access
│       └── metin2_sdk/
│           └── sdk.hpp        # Game-specific structures
├── vision_internal/           # DLL project
│   ├── CMakeLists.txt
│   ├── dllmain.cpp           # DLL entry point
│   └── logger_impl.cpp       # Logger implementation
└── vision_external/          # EXE project
    ├── CMakeLists.txt
    ├── main.cpp              # Application entry point
    ├── logger_impl.cpp       # Logger implementation
    └── ui.cpp                # UI components
```

## 🎯 Usage

### External Tool
1. Run `vision_external.exe`
2. The tool will search for `metin2client.exe`
3. If found, it will inject `vision_internal.dll`
4. Use the GUI to interact with the game

### Sample Code Examples

#### Pattern Scanning (Internal)
```cpp
#include <vision/utils/memory/memory.hpp>

c_memory main_module;
auto result = main_module.find_pattern("E8 ?? ?? ?? ?? 83 C4 08");
if (result) {
    auto addr = result.value().offset(1).relative(1, 5);
    logger::log_info("Found function at: 0x{:X}", addr.raw());
}
```

#### External Process Access
```cpp
#include <vision/utils/memory/process.hpp>

c_process::enable_debug_privileges();
auto proc = c_process::get("metin2client.exe");
if (proc) {
    auto result = proc.value().find_pattern("E8 ?? ?? ?? ?? 83 C4 08");
    if (result) {
        logger::log_info("Found pattern at: 0x{:X}", result->raw());
    }
}
```

#### Logging
```cpp
#include <vision/logger.hpp>

logger::initialize("my_component");
logger::log_info("Application started");
logger::log_warning("This is a warning: {}", some_value);
logger::log_error("Error occurred: {}", error_msg);
```

## 🔒 Security Features

### **Advanced XOR String Obfuscation**
- **Compile-time Encryption**: Strings encrypted during compilation
- **Runtime Decryption**: Only decrypted when actually used
- **Unique Keys**: Each usage gets different encryption key
- **Memory Protection**: Automatic cleanup prevents memory dumps
- **Stack-based Decryption**: Most secure option avoids heap allocation

```cpp
// Basic usage (encrypted at compile-time, decrypted at runtime)
std::string basic = xorstr("sensitive data");

// Secure usage (automatic memory cleanup)
auto secure = xorstr_secure("API key");

// Stack-based usage (most secure - never touches heap)
xorstr_call("password123", [](const char* pwd) {
    authenticate(pwd);  // Use immediately
    // Memory automatically cleared after lambda
});
```

### **Additional Security**
- **Compile-time Hashing**: `fnv::hash("function_name")`
- **Debug Privilege Elevation**: Automatic when needed
- **Safe Memory Access**: Result-based error handling
- **Anti-Analysis**: Resistant to static analysis and memory dumps

## 🎨 Code Style

- **C++20** standard with modern features
- **snake_case** for functions, variables, files
- **c_** prefix for classes (e.g., `c_memory`, `c_process`)
- **m_** prefix for member variables
- **Clang-format** enforced formatting
- **Namespace organization**: `vision::utils::memory`

## 🐛 Debugging

### Debug Build
```bash
cmake --build . --config Debug
```

### Console Output
- External tool shows console window in debug mode
- Internal DLL logs to `logs/vision_internal.log`
- External tool logs to `logs/vision_external.log`

### Visual Studio Integration
- Open `build/vision.sln` in Visual Studio
- Set `vision_external` as startup project
- Use F5 to debug with full IDE support

## ⚠️ Important Notes

1. **32-bit Only**: This project targets 32-bit Windows exclusively
2. **Clang++ Required**: Uses Clang-specific features and MS compatibility mode
3. **Administrator Rights**: May require elevated privileges for process injection
4. **ImGui Dependency**: You must provide your own ImGui installation
5. **Game Compatibility**: Designed specifically for Metin2 client

## 🤝 Contributing

1. Follow the established code style (`.clang-format`)
2. Use the `result<T>` type for error handling
3. Add logging for important operations
4. Update documentation for new features
5. Test both internal and external components

## 📄 License

This project is for educational purposes. Ensure compliance with game terms of service and local laws when using with actual games.
