# vision_external - EXE project for external process manipulation
cmake_minimum_required(VERSION 3.21)

project(vision_external VERSION 1.0.0 LANGUAGES CXX)

# Find ImGui (user-provided)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

if(EXISTS ${IMGUI_DIR})
    # ImGui source files
    set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
        ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    )
    
    # Create ImGui library
    add_library(imgui STATIC ${IMGUI_SOURCES})
    
    target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )
    
    # Link DirectX 11
    target_link_libraries(imgui PUBLIC
        d3d11
        dxgi
    )
    
    set(IMGUI_AVAILABLE TRUE)
else()
    message(WARNING "ImGui not found at ${IMGUI_DIR}. Please copy ImGui to external/imgui/")
    set(IMGUI_AVAILABLE FALSE)
endif()

# Create the EXE target
add_executable(vision_external
    main.cpp
    logger_impl.cpp
    ui.cpp
)

# Set target properties
set_target_properties(vision_external PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "vision_external"
    WIN32_EXECUTABLE TRUE
)

# Include directories
target_include_directories(vision_external PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(vision_external PRIVATE
    fmt::fmt
    nlohmann_json::nlohmann_json
    Zydis
)

# Link ImGui if available
if(IMGUI_AVAILABLE)
    target_link_libraries(vision_external PRIVATE imgui)
    target_compile_definitions(vision_external PRIVATE IMGUI_AVAILABLE)
endif()

# Windows-specific settings
if(WIN32)
    target_compile_definitions(vision_external PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    # Force 32-bit
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(vision_external PRIVATE -m32)
        target_link_options(vision_external PRIVATE -m32)
    endif()
    
    # Link Windows libraries
    target_link_libraries(vision_external PRIVATE
        kernel32
        user32
        psapi
        advapi32
    )
    
    # Console subsystem for debugging
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_target_properties(vision_external PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:CONSOLE"
        )
    endif()
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(vision_external PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -fms-extensions
        -fms-compatibility
    )
    
    # Debug configuration
    target_compile_options(vision_external PRIVATE
        $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
    )
    
    # Release configuration
    target_compile_options(vision_external PRIVATE
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )
endif()

# Set folder for Visual Studio
set_target_properties(vision_external PROPERTIES FOLDER "Vision")

# Set as startup project
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT vision_external)
