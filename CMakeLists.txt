cmake_minimum_required(VERSION 3.20)

# Auto-configure vcpkg if available
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Try common vcpkg locations if not found
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(VCPKG_PATHS
        "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
        "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"
        "C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake"
        "${CMAKE_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake"
        "C:/Users/$ENV{USERNAME}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        "C:/Users/elba/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake"
    )
    
    foreach(VCPKG_PATH ${VCPKG_PATHS})
        if(EXISTS "${VCPKG_PATH}")
            set(CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}" CACHE STRING "")
            message(STATUS "Found vcpkg toolchain: ${VCPKG_PATH}")
            break()
        endif()
    endforeach()
endif()

# Set vcpkg triplet for static linking
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "x86-windows-static" CACHE STRING "")
endif()

# Ensure static linking for all configurations
set(VCPKG_LIBRARY_LINKAGE "static" CACHE STRING "")
set(VCPKG_CRT_LINKAGE "static" CACHE STRING "")

# Map configurations to use appropriate library variants
set(CMAKE_MAP_IMPORTED_CONFIG_DEVELOPER "Release;RelWithDebInfo;MinSizeRel;Debug;")
set(CMAKE_MAP_IMPORTED_CONFIG_RELEASE "Release;RelWithDebInfo;MinSizeRel;Debug;")
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG "Debug;RelWithDebInfo;Release;MinSizeRel;")

# Configure for Windows with MSVC
if(WIN32)
    # Set runtime library before project() call
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "")
endif()

project(vision VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force 32-bit architecture
set(CMAKE_GENERATOR_PLATFORM Win32)
set(CMAKE_SIZEOF_VOID_P 4)

# Build configuration types
set(CMAKE_CONFIGURATION_TYPES "Debug;Developer;Release" CACHE STRING "" FORCE)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Developer" CACHE STRING "Choose the type of build." FORCE)
endif()

# MSVC compiler flags
if(MSVC)
    # Common MSVC flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /permissive- /MP")
    
    # Debug mode
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /MTd /DDEBUG /D_DEBUG")
    
    # Developer mode (optimized debug)
    set(CMAKE_CXX_FLAGS_DEVELOPER "/O2 /Zi /MT /DDEVELOPER /DNDEBUG")
    
    # Release mode
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MT /DRELEASE /DNDEBUG /GL")
    
    # Linker flags
    set(CMAKE_EXE_LINKER_FLAGS_DEVELOPER "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEVELOPER "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
    
    message(STATUS "Using MSVC compiler")
else()
    message(FATAL_ERROR "This project requires MSVC compiler on Windows")
endif()

# Static linking configuration
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

# Force static linking for all dependencies
set(CMAKE_LINK_SEARCH_START_STATIC ON)
set(CMAKE_LINK_SEARCH_END_STATIC ON)

# Additional static linking flags for MSVC
if(MSVC)
    # Only exclude dynamic runtime libraries, keep static ones
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcrtd.lib")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcrtd.lib")
endif()

# Find vcpkg packages
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(minhook CONFIG REQUIRED)
find_package(zydis CONFIG REQUIRED)


# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external)

# Add subdirectories
add_subdirectory(vision_internal)
add_subdirectory(vision_external)

# Visual Studio solution configuration
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT vision_external)

# Disable default CMake projects (ALL_BUILD, ZERO_CHECK, etc.)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE)

# Hide utility targets in Visual Studio
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

# Configuration summary
message(STATUS "=== Vision MT2 Build Configuration ===")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER})")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Architecture: 32-bit (x86)")
message(STATUS "vcpkg Triplet: ${VCPKG_TARGET_TRIPLET}")
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "vcpkg Toolchain: ${CMAKE_TOOLCHAIN_FILE}")
else()
    message(WARNING "vcpkg toolchain not found - packages may not be available")
endif()
message(STATUS "")
message(STATUS "Static Dependencies:")
message(STATUS "  • fmt - Modern C++ formatting library")
message(STATUS "  • nlohmann_json - JSON parsing library")
message(STATUS "  • minhook - Function hooking library")
message(STATUS "  • zydis - Disassembler library")
message(STATUS "  • imgui - Immediate mode GUI (external/)")
message(STATUS "  • d3d8/d3d9 - DirectX rendering (external/)")
message(STATUS "")
message(STATUS "Project Structure:")
message(STATUS "  • vision_external.exe - Main application")
message(STATUS "  • vision_internal.dll - Hook/feature DLL")
message(STATUS "  • Organized Visual Studio folders")
message(STATUS "  • Static linking (no external DLLs)")
message(STATUS "=====================================")

# Helper message for easy building
message(STATUS "")
message(STATUS "Build Commands:")
message(STATUS "  .\\build.ps1          # Developer build (default)")
message(STATUS "  .\\build-production.ps1  # Release build")
message(STATUS "  .\\build-debug.ps1       # Debug build")
message(STATUS "  .\\clean.ps1             # Clean build directories")
message(STATUS "")
