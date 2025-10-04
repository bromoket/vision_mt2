# vision_internal - DLL project for injection into metin2client.exe
cmake_minimum_required(VERSION 3.21)

project(vision_internal VERSION 1.0.0 LANGUAGES CXX)

# Create the DLL target
add_library(vision_internal SHARED
    dllmain.cpp
    logger_impl.cpp
)

# Set target properties
set_target_properties(vision_internal PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "vision_internal"
    SUFFIX ".dll"
)

# Include directories
target_include_directories(vision_internal PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(vision_internal PRIVATE
    fmt::fmt
    nlohmann_json::nlohmann_json
    Zydis
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(vision_internal PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        VISION_INTERNAL_EXPORTS
    )
    
    # Force 32-bit
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(vision_internal PRIVATE -m32)
        target_link_options(vision_internal PRIVATE -m32)
    endif()
    
    # Link Windows libraries
    target_link_libraries(vision_internal PRIVATE
        kernel32
        user32
        psapi
    )
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(vision_internal PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -fms-extensions
        -fms-compatibility
    )
    
    # Debug configuration
    target_compile_options(vision_internal PRIVATE
        $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
    )
    
    # Release configuration
    target_compile_options(vision_internal PRIVATE
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )
endif()

# Set folder for Visual Studio
set_target_properties(vision_internal PROPERTIES FOLDER "Vision")
