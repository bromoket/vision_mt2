project(vision_internal)

# Organize source files by category
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp" "src/core/*.hpp")
file(GLOB_RECURSE FEATURES_SOURCES "src/features/*.cpp" "src/features/*.hpp")
file(GLOB_RECURSE UTILS_SOURCES "src/utils/*.cpp" "src/utils/*.hpp")
file(GLOB_RECURSE MAIN_SOURCES "src/dllmain.cpp")

# Hook sources by category
file(GLOB_RECURSE GAME_HOOK_SOURCES "src/hooks/game/*.cpp" "src/hooks/game/*.hpp")
file(GLOB_RECURSE NETWORK_HOOK_SOURCES "src/hooks/networking/*.cpp" "src/hooks/networking/*.hpp")
file(GLOB_RECURSE PYTHON_HOOK_SOURCES "src/hooks/python/*.cpp" "src/hooks/python/*.hpp")
file(GLOB_RECURSE WINAPI_HOOK_SOURCES "src/hooks/winapi/*.cpp" "src/hooks/winapi/*.hpp")

# Header files by category
file(GLOB_RECURSE HOOK_HEADERS "include/hooks/*.hpp")
file(GLOB_RECURSE WINAPI_HEADERS "include/hooks/winapi/*.hpp")
file(GLOB_RECURSE SHARED_HEADERS "${CMAKE_SOURCE_DIR}/include/vision/*.hpp")

# Combine all sources
set(INTERNAL_SOURCES 
    ${CORE_SOURCES}
    ${FEATURES_SOURCES}
    ${UTILS_SOURCES}
    ${MAIN_SOURCES}
    ${GAME_HOOK_SOURCES}
    ${NETWORK_HOOK_SOURCES}
    ${PYTHON_HOOK_SOURCES}
    ${WINAPI_HOOK_SOURCES}
    ${HOOK_HEADERS}
    ${WINAPI_HEADERS}
    ${SHARED_HEADERS}
)

# Create the DLL
add_library(vision_internal SHARED ${INTERNAL_SOURCES})

# Link libraries
target_link_libraries(vision_internal PRIVATE 
    fmt::fmt
    nlohmann_json::nlohmann_json
    minhook::minhook
    Zydis::Zydis
)

# Include directories
target_include_directories(vision_internal PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set output directory
set_target_properties(vision_internal PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Windows specific settings
if(WIN32)
    target_compile_definitions(vision_internal PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Visual Studio folder organization
set_target_properties(vision_internal PROPERTIES FOLDER "Vision MT2")

# Create Visual Studio source groups (folders)
source_group("Source Files\\Core" FILES ${CORE_SOURCES})
source_group("Source Files\\Features" FILES ${FEATURES_SOURCES})
source_group("Source Files\\Utils" FILES ${UTILS_SOURCES})
source_group("Source Files" FILES ${MAIN_SOURCES})

source_group("Source Files\\Hooks\\Game" FILES ${GAME_HOOK_SOURCES})
source_group("Source Files\\Hooks\\Networking" FILES ${NETWORK_HOOK_SOURCES})
source_group("Source Files\\Hooks\\Python" FILES ${PYTHON_HOOK_SOURCES})
source_group("Source Files\\Hooks\\WinAPI" FILES ${WINAPI_HOOK_SOURCES})

source_group("Header Files\\Hooks" FILES ${HOOK_HEADERS})
source_group("Header Files\\Hooks\\WinAPI" FILES ${WINAPI_HEADERS})
source_group("Header Files\\Shared" FILES ${SHARED_HEADERS})
