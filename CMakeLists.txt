#
# CMakeLists.txt for TuxMath (SDL3 Edition)
#
# Provides native Windows (MinGW/MSYS2) build support alongside the
# existing Autotools build for Debian/Linux.
# All Windows-specific blocks are guarded by if(MINGW) / if(WIN32) so
# the Linux/Mac Autotools build is completely unaffected.
#
# Original CMake build: Copyright 2008-2011 Timothy E. Holy
# Windows porting: mirrors the strategy used for t4kcommon and TuxType
#
# License information can be found in doc/COPYING.txt

cmake_minimum_required(VERSION 3.10)

set(PACKAGE_NAME    "tuxmath")
set(TUXMATH_VERSION "2.0.4")
set(TUXMATHADMIN_VERSION "0.1.1")
set(APP_NAME        "TuxMath")

project(${APP_NAME})

message(STATUS "Building TuxMath version ${TUXMATH_VERSION}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

include(T4K_Sugar)

## Choose a debug build by default
t4k_gentle_set(CMAKE_BUILD_TYPE Debug)

# -------------------------------------------------------------------
# Find required libraries via pkg-config (works on both Linux + MinGW)
# -------------------------------------------------------------------
find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL3       REQUIRED IMPORTED_TARGET sdl3)
pkg_check_modules(SDL3_IMAGE REQUIRED IMPORTED_TARGET sdl3-image)
pkg_check_modules(SDL3_TTF   REQUIRED IMPORTED_TARGET sdl3-ttf)
pkg_check_modules(SDL3_MIXER REQUIRED IMPORTED_TARGET sdl3-mixer)
pkg_check_modules(SDL3_NET            IMPORTED_TARGET sdl3-net)

# Check for libRSVG / Cairo (used for SVG support)
pkg_check_modules(RSVG  librsvg-2.0)
pkg_check_modules(CAIRO cairo)

if(RSVG_FOUND AND CAIRO_FOUND)
    set(HAVE_RSVG 1)
endif()

# -------------------------------------------------------------------
# NLS / gettext
# -------------------------------------------------------------------
find_package(MSGFMT)
if(NOT DEFINED ENABLE_NLS OR NOT MSGFMT_FOUND)
    set(ENABLE_NLS ${MSGFMT_FOUND})
endif()
if(NOT ENABLE_NLS)
    message(STATUS "Internationalization disabled")
endif()
# Build bundled intl only on Apple (matches original logic)
if(ENABLE_NLS)
    if(APPLE)
        set(TUXMATH_BUILD_INTL true)
    endif()
else()
    set(TUXMATH_BUILD_INTL false)
endif()
message(STATUS "ENABLE_NLS: ${ENABLE_NLS}")
set(HAVE_GETTEXT ENABLE_NLS)

# -------------------------------------------------------------------
# Locate libt4k_common
# Strategy mirrors TuxType: pkg-config first (Linux/installed),
# then sibling-repo fallback (typical development layout).
# -------------------------------------------------------------------
pkg_check_modules(T4K_COMMON t4k_common)
if(T4K_COMMON_FOUND)
    set(HAVE_LIBT4K_COMMON 1)
else()
    # Fallback search locations: look next to this repo (standard git clone layout)
    set(_t4k_search_dirs
        "${CMAKE_SOURCE_DIR}/../t4kcommon"
        "${CMAKE_SOURCE_DIR}/../t4kcommon-master"
        "${CMAKE_SOURCE_DIR}/../../t4kcommon-master (4)/t4kcommon-master"
    )
    foreach(_cand ${_t4k_search_dirs})
        if(EXISTS "${_cand}/src/t4k_common.h")
            set(T4K_COMMON_INCLUDE_DIRS "${_cand}/src")
            set(T4K_COMMON_LIBRARY_DIRS "${_cand}/build/src" "${_cand}/build-win/src")
            set(T4K_COMMON_LIBRARIES    "t4k_common")
            set(HAVE_LIBT4K_COMMON 1)
            message(STATUS "Found t4kcommon in sibling directory: ${_cand}")
            break()
        endif()
    endforeach()

    if(NOT HAVE_LIBT4K_COMMON)
        set(T4K_COMMON_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/../t4kcommon/src")
        set(T4K_COMMON_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/../t4kcommon/build/src")
        set(T4K_COMMON_LIBRARIES    "t4k_common")
        message(STATUS "t4k_common not found via pkg-config, using default fallback: ${T4K_COMMON_LIBRARY_DIRS}")
    endif()
endif()

# SDL_gfx has no SDL3 port — vendored SDL_rotozoom.c is always used
# (handled in src/CMakeLists.txt)

# -------------------------------------------------------------------
# Platform-specific data prefix & build defines
# -------------------------------------------------------------------
if(APPLE)
    set(MACOSX_BUNDLE_NAME ${APP_NAME})
    set(TUXMATH_DATA_PREFIX "${APP_NAME}.app/Contents/Resources")
    set(LOCALE_DIR          "${APP_NAME}.app/Contents/Resources/locale")
    set(TUXMATH_EXTRA_INCLUDE "${CMAKE_SOURCE_DIR}/macosx")
    set(TUXMATH_EXTRA_SRC     "${CMAKE_SOURCE_DIR}/macosx/SDLMain.m")
    set(CMAKE_OSX_ARCHITECTURES ppc i386)
elseif(WIN32)
    # On Windows, data sits next to the .exe in the install directory
    set(TUXMATH_DATA_PREFIX "data")
    set(LOCALE_DIR          "data/locale")
    if(MINGW)
        # Activate existing MinGW-specific code paths in the source
        add_definitions(-DBUILD_MINGW32)
        add_definitions(-DPROGRAM_NAME="TuxMath")
        set(BUILD_MINGW32 1)
    endif()
elseif(UNIX)
    set(TUXMATH_DATA_PREFIX "${CMAKE_INSTALL_PREFIX}/share/tuxmath")
    set(LOCALE_DIR          "${CMAKE_INSTALL_PREFIX}/share/locale")
else()
    message(SEND_ERROR "Unknown platform!")
endif()

message(STATUS "TUXMATH_DATA_PREFIX: ${TUXMATH_DATA_PREFIX}")

# -------------------------------------------------------------------
# Optional SDL3_net
# -------------------------------------------------------------------
if(SDL3_NET_FOUND)
    set(HAVE_LIBSDL_NET 1)
    message(STATUS "SDL3_net found — building with LAN/network support")
endif()

# -------------------------------------------------------------------
# Process subdirectories
# -------------------------------------------------------------------
if(TUXMATH_BUILD_INTL)
    set(INTL_BINARY_DIR ${CMAKE_BINARY_DIR}/intl)
    set(INTL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/intl)
    set(TOP_SRC_DIR     ${CMAKE_BINARY_DIR})
    set(PREFIX          ${CMAKE_BINARY_DIR})
    add_subdirectory(intl)
endif()

add_subdirectory(src)

if(EXISTS "${CMAKE_SOURCE_DIR}/data/CMakeLists.txt")
    add_subdirectory(data)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/doc/CMakeLists.txt")
    add_subdirectory(doc)
endif()

if(ENABLE_NLS)
    if(EXISTS "${CMAKE_SOURCE_DIR}/po/CMakeLists.txt")
        set(LOCALE_INSTALL_DIR ${CMAKE_BINARY_DIR}/src/${LOCALE_DIR})
        add_subdirectory(po)
    endif()
endif()

if(APPLE)
    if(EXISTS "${CMAKE_SOURCE_DIR}/macosx/CMakeLists.txt")
        add_subdirectory(macosx)
    endif()
endif()
