project(vision_external)

# Organize source files by category
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp" "src/core/*.hpp")
file(GLOB_RECURSE INJECTION_SOURCES "src/injection/*.cpp" "src/injection/*.hpp")
file(GLOB_RECURSE MEMORY_SOURCES "src/memory/*.cpp" "src/memory/*.hpp")
file(GLOB_RECURSE UI_SOURCES "src/ui/*.cpp" "src/ui/*.hpp")
file(GLOB_RECURSE MAIN_SOURCES "src/main.cpp")

# Header files by category
file(GLOB_RECURSE MEMORY_HEADERS "include/memory/*.hpp")
file(GLOB_RECURSE UI_HEADERS "include/ui/*.hpp")
file(GLOB_RECURSE SHARED_HEADERS "${CMAKE_SOURCE_DIR}/include/vision/*.hpp")

# ImGui source files
set(IMGUI_CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
)

set(IMGUI_BACKEND_SOURCES
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_dx9.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_win32.cpp
)

# ImGui headers
file(GLOB IMGUI_HEADERS "${CMAKE_SOURCE_DIR}/external/imgui/*.h")
file(GLOB IMGUI_BACKEND_HEADERS "${CMAKE_SOURCE_DIR}/external/imgui/backends/*.h")

# Combine all sources
set(ALL_SOURCES 
    ${CORE_SOURCES} 
    ${INJECTION_SOURCES} 
    ${MEMORY_SOURCES} 
    ${UI_SOURCES} 
    ${MAIN_SOURCES}
    ${MEMORY_HEADERS}
    ${UI_HEADERS}
    ${SHARED_HEADERS}
    ${IMGUI_CORE_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
    ${IMGUI_HEADERS}
    ${IMGUI_BACKEND_HEADERS}
)

# Create the executable
add_executable(vision_external ${ALL_SOURCES})

# Link libraries
target_link_libraries(vision_external PRIVATE 
    fmt::fmt
    nlohmann_json::nlohmann_json
    d3d9
    user32
    gdi32
)

# Include directories
target_include_directories(vision_external PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set output directory
set_target_properties(vision_external PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Windows specific settings
if(WIN32)
    target_compile_definitions(vision_external PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    
    # Set subsystem to console for now
    set_target_properties(vision_external PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()

# Visual Studio folder organization
set_target_properties(vision_external PROPERTIES FOLDER "Vision MT2")

# Create Visual Studio source groups (folders)
source_group("Source Files\\Core" FILES ${CORE_SOURCES})
source_group("Source Files\\Injection" FILES ${INJECTION_SOURCES})
source_group("Source Files\\Memory" FILES ${MEMORY_SOURCES})
source_group("Source Files\\UI" FILES ${UI_SOURCES})
source_group("Source Files" FILES ${MAIN_SOURCES})

source_group("Header Files\\Memory" FILES ${MEMORY_HEADERS})
source_group("Header Files\\UI" FILES ${UI_HEADERS})
source_group("Header Files\\Shared" FILES ${SHARED_HEADERS})

source_group("External\\ImGui\\Core" FILES ${IMGUI_CORE_SOURCES})
source_group("External\\ImGui\\Backends" FILES ${IMGUI_BACKEND_SOURCES})
source_group("External\\ImGui\\Headers" FILES ${IMGUI_HEADERS})
source_group("External\\ImGui\\Backend Headers" FILES ${IMGUI_BACKEND_HEADERS})
