#
# src/CMakeLists.txt for TuxMath (SDL3 Edition)
#
# Compiles all TuxMath source files into the tuxmath (Linux) or TuxMath (Windows)
# executable. Windows/MinGW-specific blocks are guarded by if(MINGW) / if(WIN32)
# so the Debian/Linux Autotools build is completely unaffected.
#
# Strategy mirrors TuxType's src/CMakeLists.txt exactly.
#

cmake_minimum_required(VERSION 3.10)

# -------------------------------------------------------------------
# Generate config.h into the build root so that #include "config.h"
# resolves correctly from any source file (globals.h, tuxmathadmin.c, etc.)
# -------------------------------------------------------------------
if(MINGW)
    set(BUILD_MINGW32 1)
    set(HAVE_PUTENV  1)
else()
    set(BUILD_MINGW32 0)
    set(HAVE_PUTENV  0)
endif()

if(SDL3_NET_FOUND)
    set(HAVE_LIBSDL_NET 1)
else()
    set(HAVE_LIBSDL_NET 0)
endif()

if(HAVE_RSVG)
    set(HAVE_RSVG 1)
else()
    set(HAVE_RSVG 0)
endif()

if(HAVE_LIBT4K_COMMON)
    set(HAVE_LIBT4K_COMMON 1)
else()
    set(HAVE_LIBT4K_COMMON 0)
endif()

configure_file(${CMAKE_SOURCE_DIR}/config.h.cmake
               ${CMAKE_BINARY_DIR}/config.h)

# -------------------------------------------------------------------
# Core source files (from src/Makefile.am tuxmath_SOURCES)
# -------------------------------------------------------------------
set(SOURCES_TUXMATH
    audio.c
    campaign.c
    comets.c
    comets_graphics.c
    credits.c
    draw_utils.c
    factoroids.c
    factoroids_graphics.c
    fileops.c
    fileops_media.c
    frame_counter.c
    game.c
    highscore.c
    lessons.c
    mathcards.c
    menu.c
    menu_lan.c
    multiplayer.c
    mysetenv.c
    options.c
    setup.c
    titlescreen.c
    tts_toggle.c
    tuxmath.c
    SDL_rotozoom.c
)

# Network support — include when SDL3_net is available
if(SDL3_NET_FOUND)
    list(APPEND SOURCES_TUXMATH network.c server.c)
    message(STATUS "SDL3_net found — including network.c and server.c")
endif()

# -------------------------------------------------------------------
# Executable name: TuxMath on Windows, tuxmath on Linux/Mac
# -------------------------------------------------------------------
if(WIN32)
    set(EXE_NAME "TuxMath")
else()
    set(EXE_NAME "tuxmath")
endif()

add_executable(${EXE_NAME} ${SOURCES_TUXMATH})

# -------------------------------------------------------------------
# Compile definitions (mirrors Makefile.am AM_CFLAGS / AM_CPPFLAGS)
# -------------------------------------------------------------------
target_compile_definitions(${EXE_NAME} PRIVATE
    DATA_PREFIX="${TUXMATH_DATA_PREFIX}"
    LOCALEDIR="${LOCALE_DIR}"
    VERSION="${TUXMATH_VERSION}"
    PACKAGE="tuxmath"
    DEBUG
)

if(HAVE_RSVG)
    target_compile_definitions(${EXE_NAME} PRIVATE HAVE_RSVG=1)
endif()

if(HAVE_LIBT4K_COMMON)
    target_compile_definitions(${EXE_NAME} PRIVATE HAVE_LIBT4K_COMMON=1)
endif()

if(ENABLE_NLS)
    target_compile_definitions(${EXE_NAME} PRIVATE ENABLE_NLS=1)
endif()

if(MINGW)
    target_compile_definitions(${EXE_NAME} PRIVATE
        BUILD_MINGW32=1
        HAVE_PUTENV=1
    )
endif()

if(SDL3_NET_FOUND)
    target_compile_definitions(${EXE_NAME} PRIVATE HAVE_LIBSDL_NET=1)
endif()

# -------------------------------------------------------------------
# Include directories
# -------------------------------------------------------------------
target_include_directories(${EXE_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}           # for generated config.h
    ${CMAKE_CURRENT_SOURCE_DIR}   # for local headers
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_IMAGE_INCLUDE_DIRS}
    ${SDL3_TTF_INCLUDE_DIRS}
    ${SDL3_MIXER_INCLUDE_DIRS}
    ${T4K_COMMON_INCLUDE_DIRS}
)

if(TUXMATH_BUILD_INTL)
    target_include_directories(${EXE_NAME} PRIVATE ${INTL_BINARY_DIR})
endif()

# -------------------------------------------------------------------
# On MinGW: auto-detect librsvg, cairo, glib-2.0, libintl, libiconv,
# libpng, zlib using pkg-config + compiler-derived MSYS2 prefix.
# No paths are hardcoded — works on any MSYS2 installation.
# -------------------------------------------------------------------
if(MINGW)
    pkg_check_modules(LIBRSVG REQUIRED librsvg-2.0)
    pkg_check_modules(CAIRO   REQUIRED cairo)
    pkg_check_modules(GLIB    REQUIRED glib-2.0)

    # Derive MSYS2 prefix from compiler location
    if(DEFINED ENV{MSYS2_PREFIX})
        set(_msys2_prefix "$ENV{MSYS2_PREFIX}")
    else()
        get_filename_component(_msys2_prefix "${CMAKE_C_COMPILER}" DIRECTORY)
        get_filename_component(_msys2_prefix "${_msys2_prefix}" DIRECTORY)
    endif()

    find_library(LIBINTL_LIBRARIES  NAMES intl libintl
        HINTS "${_msys2_prefix}/lib" REQUIRED)
    find_library(LIBICONV_LIBRARIES NAMES iconv libiconv
        HINTS "${_msys2_prefix}/lib" REQUIRED)
    find_library(LIBPNG_LIBRARIES   NAMES png libpng png16 libpng16
        HINTS "${_msys2_prefix}/lib" REQUIRED)
    find_library(ZLIB_LIBRARIES     NAMES z zlib
        HINTS "${_msys2_prefix}/lib" REQUIRED)

    target_include_directories(${EXE_NAME} PRIVATE
        ${LIBRSVG_INCLUDE_DIRS}
        ${CAIRO_INCLUDE_DIRS}
        ${GLIB_INCLUDE_DIRS}
    )

    link_directories(${LIBRSVG_LIBRARY_DIRS})
endif()

# -------------------------------------------------------------------
# Link libraries — common to all platforms
# -------------------------------------------------------------------
target_link_directories(${EXE_NAME} PRIVATE ${T4K_COMMON_LIBRARY_DIRS})

target_link_libraries(${EXE_NAME}
    PkgConfig::SDL3
    PkgConfig::SDL3_IMAGE
    PkgConfig::SDL3_TTF
    PkgConfig::SDL3_MIXER
    ${T4K_COMMON_LIBRARIES}
)

# RSVG + Cairo (both Linux and Windows when available)
if(RSVG_FOUND AND CAIRO_FOUND)
    target_link_libraries(${EXE_NAME}
        ${RSVG_LIBRARIES}
        ${CAIRO_LIBRARIES}
    )
endif()

# SDL3_net (optional, for LAN multiplayer)
if(SDL3_NET_FOUND)
    target_link_libraries(${EXE_NAME} PkgConfig::SDL3_NET)
    target_include_directories(${EXE_NAME} PRIVATE ${SDL3_NET_INCLUDE_DIRS})
endif()

# -------------------------------------------------------------------
# On MinGW: also link gettext, iconv, png, zlib, Windows system libs
# shlwapi is needed because compiler.h uses shlwapi.h for StrStrI
# (strcasestr replacement on Windows)
# -------------------------------------------------------------------
if(MINGW)
    target_link_libraries(${EXE_NAME}
        ${LIBRSVG_LIBRARIES}
        ${LIBINTL_LIBRARIES}
        ${LIBICONV_LIBRARIES}
        ${LIBPNG_LIBRARIES}
        ${ZLIB_LIBRARIES}
        # Required Windows system libraries
        uuid ole32 wsock32 shlwapi
    )
    # Uncomment to suppress the console window in the final .exe:
    # set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "-mwindows")
endif()

# On Linux/Mac, link math library
if(UNIX)
    target_link_libraries(${EXE_NAME} m)
endif()

# intl for Apple / build-bundled-intl path
if(TUXMATH_BUILD_INTL)
    target_link_libraries(${EXE_NAME} libintl.a)
endif()

# -------------------------------------------------------------------
# Install (Linux/Mac only — Windows uses the build directory directly)
# -------------------------------------------------------------------
if(UNIX AND NOT APPLE)
    install(TARGETS ${EXE_NAME} DESTINATION bin)
endif()

# -------------------------------------------------------------------
# Copy data folder on Windows so the game can run from build dir
# -------------------------------------------------------------------
if(WIN32)
    add_custom_command(TARGET ${EXE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/data"
            "$<TARGET_FILE_DIR:${EXE_NAME}>/data"
        COMMENT "Copying data/ folder next to TuxMath.exe..."
    )
endif()

# -------------------------------------------------------------------
# Unix-only auxiliary executables (not built on Windows)
# Matches Makefile.am: if BUILD_MINGW32 → only TuxMath + tuxmathserver
# -------------------------------------------------------------------
if(NOT WIN32)
    # tuxmathadmin
    add_executable(tuxmathadmin tuxmathadmin.c)
    target_include_directories(tuxmathadmin PRIVATE
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${T4K_COMMON_INCLUDE_DIRS}
    )
    target_compile_definitions(tuxmathadmin PRIVATE
        DATA_PREFIX="${TUXMATH_DATA_PREFIX}"
        VERSION="${TUXMATHADMIN_VERSION}"
        LOCALEDIR="${LOCALE_DIR}"
        PACKAGE="tuxmathadmin"
    )
    target_link_directories(tuxmathadmin PRIVATE ${T4K_COMMON_LIBRARY_DIRS})
    target_link_libraries(tuxmathadmin m)
    install(TARGETS tuxmathadmin RUNTIME DESTINATION bin)

    # generate_lesson
    add_executable(generate_lesson
        generate_lesson.c
        mathcards.c
        options.c
        fileops.c
        lessons.c
    )
    target_include_directories(generate_lesson PRIVATE
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${T4K_COMMON_INCLUDE_DIRS}
    )
    target_compile_definitions(generate_lesson PRIVATE
        DATA_PREFIX="${TUXMATH_DATA_PREFIX}"
        VERSION="${TUXMATH_VERSION}"
        LOCALEDIR="${LOCALE_DIR}"
        PACKAGE="tuxmath"
    )
    target_link_directories(generate_lesson PRIVATE ${T4K_COMMON_LIBRARY_DIRS})
    target_link_libraries(generate_lesson
        PkgConfig::SDL3
        ${T4K_COMMON_LIBRARIES}
        m
    )
endif()
