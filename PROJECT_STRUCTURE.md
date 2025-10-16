# Vision MT2 - Project Structure

```
C:.
├───build/                          # Build output directory
│   ├───bin/
│   │   └───Developer/              # Developer build binaries
│   ├───CMakeFiles/                 # CMake generated files
│   ├───vcpkg_installed/            # vcpkg static libraries
│   │   └───x86-windows-static/     # 32-bit static libraries
│   │       ├───debug/lib/          # Debug libraries
│   │       ├───include/            # Header files
│   │       │   ├───fmt/            # fmt library headers
│   │       │   ├───nlohmann/       # JSON library headers
│   │       │   ├───Zycore/         # Zydis core headers
│   │       │   └───Zydis/          # Zydis disassembler headers
│   │       ├───lib/                # Release libraries
│   │       └───share/              # CMake config files
│   ├───vision_external/            # External project build files
│   └───vision_internal/            # Internal project build files
├───docs/                           # Documentation
├───external/                       # Third-party dependencies
│   ├───d3d8/                       # DirectX 8 SDK
│   │   ├───include/
│   │   └───lib/
│   ├───imgui/                      # Dear ImGui library
│   │   ├───backends/
│   │   ├───docs/
│   │   ├───examples/
│   │   └───misc/
│   └───Python-2.7/                # Python 2.7 headers/libs
│       ├───include/
│       └───lib/
├───fonts/                          # Font resources
├───include/                        # Project headers
│   └───vision/
│       ├───common/                 # Common utilities
│       ├───config/                 # Configuration
│       └───utils/                  # Utility functions
├───vcpkg_installed/                # vcpkg installation (root)
│   └───x86-windows-static/         # Static libraries for x86
├───vision_external/                # Main executable project
│   ├───include/
│   │   ├───memory/                 # Memory management
│   │   └───ui/                     # User interface
│   ├───resources/                  # Resources (icons, etc.)
│   └───src/
│       ├───core/                   # Core functionality
│       ├───injection/              # DLL injection
│       ├───memory/                 # Memory operations
│       └───ui/                     # UI implementation
└───vision_internal/                # DLL project (hooks/features)
    ├───include/
    │   └───hooks/
    │       └───winapi/             # Windows API hooks
    └───src/
        ├───core/                   # Core DLL functionality
        ├───features/               # Game features
        ├───hooks/                  # Hook implementations
        │   ├───game/               # Game-specific hooks
        │   ├───networking/         # Network hooks
        │   ├───python/             # Python integration
        │   └───winapi/             # Windows API hooks
        └───utils/                  # Utility functions
```

## Build Outputs

- **vision_external.exe** - Main application executable
- **vision_internal.dll** - Hook/feature DLL injected into target process

## Dependencies (Static Linked)

- **fmt** - Modern C++ formatting library
- **nlohmann_json** - JSON parsing library  
- **minhook** - Function hooking library
- **zydis** - Disassembler library
- **imgui** - Immediate mode GUI library
- **d3d8** - DirectX 8 for rendering

## Build Configurations

- **Developer** - Optimized with debug info (`/O2 /Zi /MT`)
- **Release** - Full optimization (`/O2 /MT /GL /LTCG`)  
- **Debug** - No optimization, full debug (`/Od /Zi /MTd`)

## Visual Studio Organization

The solution is beautifully organized with:
- **Vision MT2** folder containing both projects
- **Source Files** organized by functionality (Core, UI, Hooks, etc.)
- **Header Files** separated and categorized
- **External** dependencies clearly marked (ImGui organized by component)
- **No CMake clutter** (ALL_BUILD, ZERO_CHECK hidden)
- **vision_external** set as startup project

See `VISUAL_STUDIO_STRUCTURE.md` for detailed Visual Studio layout.
