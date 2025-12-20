# FindSDL2.cmake - Find SDL2 libraries
# This module uses pkg-config or finds SDL2 manually

# Try pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(SDL2 QUIET sdl2)
    pkg_check_modules(SDL2_IMAGE QUIET SDL2_image)
    pkg_check_modules(SDL2_MIXER QUIET SDL2_mixer)
    pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
endif()

# If pkg-config didn't work, try find_package with CONFIG mode
if(NOT SDL2_FOUND)
    find_package(SDL2 CONFIG QUIET)
endif()

if(NOT SDL2_IMAGE_FOUND)
    find_package(SDL2_image CONFIG QUIET)
endif()

if(NOT SDL2_MIXER_FOUND)
    find_package(SDL2_mixer CONFIG QUIET)
endif()

if(NOT SDL2_TTF_FOUND)
    find_package(SDL2_ttf CONFIG QUIET)
endif()

# Manual search fallback
if(NOT SDL2_FOUND)
    find_path(SDL2_INCLUDE_DIRS SDL.h
        HINTS
            /usr/include/SDL2
            /usr/local/include/SDL2
            $ENV{SDL2DIR}/include
    )

    find_library(SDL2_LIBRARIES
        NAMES SDL2
        HINTS
            /usr/lib
            /usr/local/lib
            $ENV{SDL2DIR}/lib
    )

    if(SDL2_INCLUDE_DIRS AND SDL2_LIBRARIES)
        set(SDL2_FOUND TRUE)
    endif()
endif()

# Create imported targets if not already created
if(SDL2_FOUND AND NOT TARGET SDL2::SDL2)
    add_library(SDL2::SDL2 INTERFACE IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${SDL2_LIBRARIES}"
    )

    add_library(SDL2::SDL2main INTERFACE IMPORTED)
endif()

if(SDL2_IMAGE_FOUND AND NOT TARGET SDL2_image::SDL2_image)
    add_library(SDL2_image::SDL2_image INTERFACE IMPORTED)
    set_target_properties(SDL2_image::SDL2_image PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${SDL2_IMAGE_LIBRARIES}"
    )
endif()

if(SDL2_MIXER_FOUND AND NOT TARGET SDL2_mixer::SDL2_mixer)
    add_library(SDL2_mixer::SDL2_mixer INTERFACE IMPORTED)
    set_target_properties(SDL2_mixer::SDL2_mixer PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_MIXER_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${SDL2_MIXER_LIBRARIES}"
    )
endif()

if(SDL2_TTF_FOUND AND NOT TARGET SDL2_ttf::SDL2_ttf)
    add_library(SDL2_ttf::SDL2_ttf INTERFACE IMPORTED)
    set_target_properties(SDL2_ttf::SDL2_ttf PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${SDL2_TTF_LIBRARIES}"
    )
endif()

# Report findings
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 DEFAULT_MSG SDL2_LIBRARIES SDL2_INCLUDE_DIRS)
find_package_handle_standard_args(SDL2_image DEFAULT_MSG SDL2_IMAGE_LIBRARIES)
find_package_handle_standard_args(SDL2_mixer DEFAULT_MSG SDL2_MIXER_LIBRARIES)
find_package_handle_standard_args(SDL2_ttf DEFAULT_MSG SDL2_TTF_LIBRARIES)
