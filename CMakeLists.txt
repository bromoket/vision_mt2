cmake_minimum_required(VERSION 3.21)

# Set vcpkg toolchain before project declaration
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

project(vision VERSION 1.0.0 LANGUAGES CXX)

# Force 32-bit architecture
set(CMAKE_GENERATOR_PLATFORM Win32)
set(CMAKE_SIZEOF_VOID_P 4)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force Clang++ compiler
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(WARNING "This project is designed for Clang++. Current compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(
        -m32                    # Force 32-bit
        -Wall                   # Enable warnings
        -Wextra                 # Extra warnings
        -Wpedantic              # Pedantic warnings
        -fms-extensions         # Microsoft extensions for Windows
        -fms-compatibility      # MS compatibility mode
    )
    
    # Debug configuration
    add_compile_options($<$<CONFIG:Debug>:-g -O0 -DDEBUG>)
    
    # Release configuration
    add_compile_options($<$<CONFIG:Release>:-O3 -DNDEBUG>)
endif()

# Windows-specific settings
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Find required packages
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Zydis CONFIG REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add subdirectories
add_subdirectory(vision_internal)
add_subdirectory(vision_external)

# Set startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT vision_external)

# Create solution folders for Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(vision_internal PROPERTIES FOLDER "Vision")
set_target_properties(vision_external PROPERTIES FOLDER "Vision")
