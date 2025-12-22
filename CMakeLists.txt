cmake_minimum_required(VERSION 3.16)
project(TankGame VERSION 1.0.0 LANGUAGES CXX)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include compiler settings
include(cmake/CompilerSettings.cmake)

# Add CMake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Find SDL2 and related libraries
if(CMAKE_CROSSCOMPILING AND WIN32 AND NOT VCPKG_TOOLCHAIN)
    # Cross-compiling with MinGW: SDL2 paths are set in toolchain file
    # Create imported targets manually
    if(NOT TARGET SDL2::SDL2)
        add_library(SDL2::SDL2 INTERFACE IMPORTED)
        set_target_properties(SDL2::SDL2 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${SDL2_LIBRARIES}"
        )
    endif()

    if(NOT TARGET SDL2::SDL2main)
        add_library(SDL2::SDL2main INTERFACE IMPORTED)
    endif()

    if(NOT TARGET SDL2_image::SDL2_image)
        add_library(SDL2_image::SDL2_image INTERFACE IMPORTED)
        set_target_properties(SDL2_image::SDL2_image PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${SDL2_IMAGE_LIBRARIES}"
        )
    endif()

    if(NOT TARGET SDL2_mixer::SDL2_mixer)
        add_library(SDL2_mixer::SDL2_mixer INTERFACE IMPORTED)
        set_target_properties(SDL2_mixer::SDL2_mixer PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_MIXER_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${SDL2_MIXER_LIBRARIES}"
        )
    endif()

    if(NOT TARGET SDL2_ttf::SDL2_ttf)
        add_library(SDL2_ttf::SDL2_ttf INTERFACE IMPORTED)
        set_target_properties(SDL2_ttf::SDL2_ttf PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${SDL2_TTF_LIBRARIES}"
        )
    endif()
else()
    # Native build or vcpkg: use find_package
    find_package(SDL2 REQUIRED CONFIG)
    find_package(SDL2_image REQUIRED CONFIG)
    find_package(SDL2_mixer REQUIRED CONFIG)
    find_package(SDL2_ttf REQUIRED CONFIG)
endif()

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

# Link libraries
if(CMAKE_CROSSCOMPILING AND WIN32 AND NOT VCPKG_TOOLCHAIN)
    # Windows cross-compile with MinGW: need specific link order for SDL2
    # Order: mingw32 -> SDL2main -> SDL2 -> extensions
    target_link_libraries(${PROJECT_NAME} PRIVATE
        -lmingw32
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
    )
else()
    # Native build or vcpkg: use imported targets
    target_link_libraries(${PROJECT_NAME} PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    )
endif()

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copying assets to build directory"
)

# Unit tests (optional)
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "=== TankGame Configuration ===")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  SDL2 Found: ${SDL2_FOUND}")
message(STATUS "  Cross-Compiling: ${CMAKE_CROSSCOMPILING}")
message(STATUS "")
