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

# Search paths configuration
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Static linking for runtime libraries
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

# Skip SDL2 configuration during compiler detection
if(CMAKE_CROSSCOMPILING)
    # SDL2 versions
    set(SDL2_VERSION "2.30.10")
    set(SDL2_IMAGE_VERSION "2.8.4")
    set(SDL2_MIXER_VERSION "2.8.0")
    set(SDL2_TTF_VERSION "2.22.0")

    # Get the actual source directory (not the try_compile temp dir)
    get_filename_component(_TOOLCHAIN_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
    get_filename_component(_SOURCE_DIR "${_TOOLCHAIN_DIR}" DIRECTORY)

    # SDL2 paths
    set(SDL2_ROOT "${_SOURCE_DIR}/deps/win64/SDL2-${SDL2_VERSION}/x86_64-w64-mingw32")
    set(SDL2_IMAGE_ROOT "${_SOURCE_DIR}/deps/win64/SDL2_image-${SDL2_IMAGE_VERSION}/x86_64-w64-mingw32")
    set(SDL2_MIXER_ROOT "${_SOURCE_DIR}/deps/win64/SDL2_mixer-${SDL2_MIXER_VERSION}/x86_64-w64-mingw32")
    set(SDL2_TTF_ROOT "${_SOURCE_DIR}/deps/win64/SDL2_ttf-${SDL2_TTF_VERSION}/x86_64-w64-mingw32")

    # Set include and library paths for find_package
    # Note: SDL2 extensions need both include/ and include/SDL2/ paths
    set(SDL2_INCLUDE_DIRS "${SDL2_ROOT}/include;${SDL2_ROOT}/include/SDL2")
    # Link order: mingw32 -> SDL2main -> SDL2
    set(SDL2_LIBRARIES
        "${SDL2_ROOT}/lib/libSDL2main.a"
        "${SDL2_ROOT}/lib/libSDL2.dll.a"
    )

    set(SDL2_IMAGE_INCLUDE_DIRS "${SDL2_IMAGE_ROOT}/include;${SDL2_IMAGE_ROOT}/include/SDL2")
    set(SDL2_IMAGE_LIBRARIES "${SDL2_IMAGE_ROOT}/lib/libSDL2_image.dll.a")

    set(SDL2_MIXER_INCLUDE_DIRS "${SDL2_MIXER_ROOT}/include;${SDL2_MIXER_ROOT}/include/SDL2")
    set(SDL2_MIXER_LIBRARIES "${SDL2_MIXER_ROOT}/lib/libSDL2_mixer.dll.a")

    set(SDL2_TTF_INCLUDE_DIRS "${SDL2_TTF_ROOT}/include;${SDL2_TTF_ROOT}/include/SDL2")
    set(SDL2_TTF_LIBRARIES "${SDL2_TTF_ROOT}/lib/libSDL2_ttf.dll.a")

    # Mark packages as found
    set(SDL2_FOUND TRUE)
    set(SDL2_image_FOUND TRUE)
    set(SDL2_mixer_FOUND TRUE)
    set(SDL2_ttf_FOUND TRUE)

    # Print configuration (only once)
    if(NOT _TOOLCHAIN_CONFIGURED)
        set(_TOOLCHAIN_CONFIGURED TRUE CACHE INTERNAL "")
        message(STATUS "")
        message(STATUS "=== Cross-Compile Configuration ===")
        message(STATUS "  Target: Windows x86_64")
        message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER}")
        message(STATUS "  SDL2: ${SDL2_ROOT}")
        message(STATUS "")
    endif()
endif()
