# CMake Toolchain file for cross-compiling to Windows x64
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-mingw64.cmake ..

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Compiler settings
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Target environment
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# SDL2 versions
set(SDL2_VERSION "2.30.10")
set(SDL2_IMAGE_VERSION "2.8.4")
set(SDL2_MIXER_VERSION "2.8.0")
set(SDL2_TTF_VERSION "2.22.0")

# SDL2 paths
set(SDL2_DIR "${CMAKE_SOURCE_DIR}/deps/win64/SDL2-${SDL2_VERSION}/x86_64-w64-mingw32")
set(SDL2_IMAGE_DIR "${CMAKE_SOURCE_DIR}/deps/win64/SDL2_image-${SDL2_IMAGE_VERSION}/x86_64-w64-mingw32")
set(SDL2_MIXER_DIR "${CMAKE_SOURCE_DIR}/deps/win64/SDL2_mixer-${SDL2_MIXER_VERSION}/x86_64-w64-mingw32")
set(SDL2_TTF_DIR "${CMAKE_SOURCE_DIR}/deps/win64/SDL2_ttf-${SDL2_TTF_VERSION}/x86_64-w64-mingw32")

# Verify SDL2 directories exist
if(NOT EXISTS "${SDL2_DIR}")
    message(FATAL_ERROR
        "SDL2 not found at ${SDL2_DIR}\n"
        "Please run: ./scripts/download-sdl2-win64.sh"
    )
endif()

# Set include and library paths
set(SDL2_INCLUDE_DIRS "${SDL2_DIR}/include/SDL2")
set(SDL2_LIBRARIES "${SDL2_DIR}/lib/libSDL2.dll.a;${SDL2_DIR}/lib/libSDL2main.a")

set(SDL2_IMAGE_INCLUDE_DIRS "${SDL2_IMAGE_DIR}/include/SDL2")
set(SDL2_IMAGE_LIBRARIES "${SDL2_IMAGE_DIR}/lib/libSDL2_image.dll.a")

set(SDL2_MIXER_INCLUDE_DIRS "${SDL2_MIXER_DIR}/include/SDL2")
set(SDL2_MIXER_LIBRARIES "${SDL2_MIXER_DIR}/lib/libSDL2_mixer.dll.a")

set(SDL2_TTF_INCLUDE_DIRS "${SDL2_TTF_DIR}/include/SDL2")
set(SDL2_TTF_LIBRARIES "${SDL2_TTF_DIR}/lib/libSDL2_ttf.dll.a")

# Create imported targets
add_library(SDL2::SDL2 STATIC IMPORTED)
set_target_properties(SDL2::SDL2 PROPERTIES
    IMPORTED_LOCATION "${SDL2_DIR}/lib/libSDL2.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
)

add_library(SDL2::SDL2main STATIC IMPORTED)
set_target_properties(SDL2::SDL2main PROPERTIES
    IMPORTED_LOCATION "${SDL2_DIR}/lib/libSDL2main.a"
)

add_library(SDL2_image::SDL2_image STATIC IMPORTED)
set_target_properties(SDL2_image::SDL2_image PROPERTIES
    IMPORTED_LOCATION "${SDL2_IMAGE_DIR}/lib/libSDL2_image.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_INCLUDE_DIRS}"
)

add_library(SDL2_mixer::SDL2_mixer STATIC IMPORTED)
set_target_properties(SDL2_mixer::SDL2_mixer PROPERTIES
    IMPORTED_LOCATION "${SDL2_MIXER_DIR}/lib/libSDL2_mixer.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_MIXER_INCLUDE_DIRS}"
)

add_library(SDL2_ttf::SDL2_ttf STATIC IMPORTED)
set_target_properties(SDL2_ttf::SDL2_ttf PROPERTIES
    IMPORTED_LOCATION "${SDL2_TTF_DIR}/lib/libSDL2_ttf.dll.a"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_INCLUDE_DIRS}"
)

# Mark packages as found
set(SDL2_FOUND TRUE)
set(SDL2_image_FOUND TRUE)
set(SDL2_mixer_FOUND TRUE)
set(SDL2_ttf_FOUND TRUE)

# Search paths configuration
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Static linking for runtime libraries (no need for libgcc/libstdc++ DLLs)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

# Windows subsystem (set to "windows" to hide console, "console" to show it)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

# Print configuration
message(STATUS "")
message(STATUS "=== Cross-Compile Configuration ===")
message(STATUS "  Target: Windows x86_64")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  SDL2: ${SDL2_DIR}")
message(STATUS "")
