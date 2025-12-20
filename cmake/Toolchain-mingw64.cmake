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

# SDL2 paths (adjust based on where SDL2 is extracted)
set(SDL2_DIR "${CMAKE_SOURCE_DIR}/deps/win64/SDL2-2.30.10/x86_64-w64-mingw32")
set(SDL2_IMAGE_DIR "${CMAKE_SOURCE_DIR}/deps/win64/SDL2_image-2.8.4/x86_64-w64-mingw32")
set(SDL2_MIXER_DIR "${CMAKE_SOURCE_DIR}/deps/win64/SDL2_mixer-2.8.0/x86_64-w64-mingw32")
set(SDL2_TTF_DIR "${CMAKE_SOURCE_DIR}/deps/win64/SDL2_ttf-2.22.0/x86_64-w64-mingw32")

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Static linking for runtime libraries
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
