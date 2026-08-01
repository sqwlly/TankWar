# Copies the runtime DLL dependencies of EXE next to it.
# Used as a POST_BUILD step for native MinGW builds so the game can be
# launched without having the MSYS2 bin directory on PATH.
#
# Expected -D arguments:
#   EXE      - path to the built executable
#   DEST     - directory to copy DLLs into (usually the exe's directory)
#   DLL_DIRS - semicolon-separated list of directories to resolve DLLs from

file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES "${EXE}"
    RESOLVED_DEPENDENCIES_VAR resolved
    UNRESOLVED_DEPENDENCIES_VAR unresolved
    CONFLICTING_DEPENDENCIES_PREFIX conflicts
    DIRECTORIES "${DLL_DIRS}"
    PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
    POST_EXCLUDE_REGEXES "[Ww][Ii][Nn][Dd][Oo][Ww][Ss][/\\\\][Ss]ystem32"
)

foreach(dll IN LISTS resolved)
    file(COPY "${dll}" DESTINATION "${DEST}")
endforeach()

list(LENGTH resolved count)
message(STATUS "Copied ${count} runtime DLLs to ${DEST}")
if(unresolved)
    message(STATUS "Unresolved (system) DLLs left to the OS: ${unresolved}")
endif()
