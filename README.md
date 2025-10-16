# Vision MT2

> **⚠️ DEVELOPER REPOSITORY** - This is a development-focused game enhancement tool. Use responsibly and in accordance with applicable terms of service.

A modern C++ game enhancement framework with static linking, clean architecture, and professional Visual Studio integration.

## 🚀 Developer Quick Start

### Prerequisites
- **Visual Studio 2019/2022** with C++ development tools
- **vcpkg** (automatically detected if installed)
- **Windows 10/11** (32-bit builds)
- **Git** for version control

### Build Commands

```powershell
# Clone and setup
git clone https://github.com/bromoket/visual_mt2.git
cd visual_mt2

# Developer build (optimized + debug info) - Default
.\build.ps1

# Production release (fully optimized)
.\build-production.ps1

# Debug build (no optimization, full debug symbols)
.\build-debug.ps1

# Clean all build artifacts
.\clean.ps1
```

## 🏗️ Architecture Overview

### Core Components
- **`vision_external.exe`** - Main application (UI, injection, memory management)
- **`vision_internal.dll`** - Hook DLL (game features, API hooks, Python integration)

### Project Structure
```
visual_mt2/
├── vision_external/     # Main executable source
│   ├── src/core/        # Core functionality
│   ├── src/injection/   # DLL injection system
│   ├── src/memory/      # Memory management
│   └── src/ui/          # ImGui interface
├── vision_internal/     # Hook DLL source
│   ├── src/hooks/       # API hooks (game, network, WinAPI)
│   ├── src/features/    # Game enhancement features
│   └── src/core/        # DLL core functionality
├── external/            # Third-party dependencies
├── include/             # Shared headers
└── build/               # Build output (gitignored)
```

## ⚙️ Technical Specifications

### Build Configurations
| Configuration | Optimization | Debug Info | Runtime | Use Case |
|---------------|-------------|------------|---------|----------|
| **Developer** | `/O2` | `/Zi` | `/MT` | Development & Testing |
| **Release** | `/O2 /GL /LTCG` | None | `/MT` | Production Distribution |
| **Debug** | `/Od` | `/Zi` | `/MTd` | Debugging Issues |

### Static Dependencies (vcpkg)
- **fmt** - Modern C++ formatting library
- **nlohmann_json** - JSON parsing and serialization
- **minhook** - Function hooking and detours
- **zydis** - x86/x64 disassembler library

### External Dependencies
- **ImGui** - Immediate mode GUI framework
- **DirectX 8/9** - Graphics rendering
- **Python 2.7** - Scripting integration

### Architecture Details
- **32-bit (x86)** builds only
- **Static linking** for all dependencies (no external DLLs)
- **MSVC runtime** statically linked
- **Clean Visual Studio integration** with organized folders

## 🛠️ Development Workflow

### Visual Studio Integration
```powershell
.\build.ps1                    # Generate solution
start build/vision.sln         # Open in Visual Studio
```

**Solution Features:**
- ✅ Clean, organized folder structure
- ✅ No CMake utility projects (ALL_BUILD, ZERO_CHECK hidden)
- ✅ `vision_external` set as startup project
- ✅ Source files organized by functionality
- ✅ External dependencies clearly marked

### Debugging
1. Build in **Debug** configuration: `.\build-debug.ps1`
2. Open `build/vision.sln` in Visual Studio
3. Set breakpoints in organized source folders
4. Press F5 to start debugging

### Adding Features
1. **Game hooks** → `vision_internal/src/hooks/game/`
2. **UI components** → `vision_external/src/ui/`
3. **Memory operations** → `vision_external/src/memory/`
4. **Core features** → `vision_internal/src/features/`

## 📊 Build Output

```
build/bin/[Configuration]/
├── vision_external.exe    # Main application
└── vision_internal.dll    # Hook DLL
```

## 🔧 Advanced Configuration

### vcpkg Integration
The project automatically detects vcpkg installations from:
- `%VCPKG_ROOT%` environment variable
- Common installation paths (`C:/vcpkg`, `C:/dev/vcpkg`, etc.)
- Local `./vcpkg/` directory

### Custom Build Flags
Modify `CMakeLists.txt` for custom compiler flags or additional dependencies.

## 📚 Documentation

- **`PROJECT_STRUCTURE.md`** - Detailed project layout
- **`VISUAL_STUDIO_STRUCTURE.md`** - Visual Studio organization guide
- **Source code** - Extensively commented for developers

## ⚠️ Legal Notice

This tool is for educational and development purposes. Users are responsible for compliance with applicable terms of service and local laws. The developers assume no responsibility for misuse.

---

**Developed by:** [@bromoket](https://github.com/bromoket)  
**License:** Educational/Development Use  
**Platform:** Windows (32-bit)
