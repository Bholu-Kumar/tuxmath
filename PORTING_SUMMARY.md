# TuxMath Windows Porting & Verification — Summary Note

## Overview
TuxMath has been ported to native Windows (via MSYS2 / MinGW-w64 UCRT64 and CMake) while strictly keeping the codebase in pure C, maintaining full compatibility with the existing Ubuntu Autotools build, and adhering to the incremental porting strategy established by `t4kcommon` and `tuxtype`.

---

## 1. Build System Updates (CMake)

- **Root `CMakeLists.txt`**:
  - Replaced legacy CMake package lookup with `pkg_check_modules` for SDL3 libraries (`sdl3`, `sdl3-image`, `sdl3-ttf`, `sdl3-mixer`), `librsvg-2.0`, `cairo`, and `libxml-2.0`.
  - Added Windows/MinGW detection logic (`WIN32`, `MINGW`) that defines `-DBUILD_MINGW32` and sets data directories to relative paths (`data`).
- **Source `src/CMakeLists.txt`**:
  - Defined target executable as `TuxMath.exe` on Windows (`tuxmath` on Unix).
  - Linked essential Windows subsystem libraries (`uuid`, `ole32`, `wsock32`, `shlwapi`).
  - Guarded Unix-only administrative utilities (`tuxmathadmin`, `generate_lesson`) behind `if(NOT WIN32)`.
  - Added automatic post-build copying of the `data/` directory next to `TuxMath.exe`.
- **Configuration Header `config.h.cmake`**:
  - Expanded template to generate all necessary macros (`BUILD_MINGW32`, `HAVE_RSVG`, `HAVE_LIBT4K_COMMON`, `ENABLE_NLS`, etc.).
  - Changed `#cmakedefine01` to `#cmakedefine ... 1` so optional/absent features generate `/* #undef */` instead of `#define ... 0` (preventing `#ifdef` collisions in C).

---

## 2. C Source Code Portability & Guards

All modifications use preprocessor `#ifdef` guards to avoid impacting Linux/Ubuntu:

1. **`src/compiler.h`**:
   - Aligned the `WIN32` block with `tuxtype`.
   - Guarded the Windows 98 `isspace` macro under classic MinGW only, preventing conflicts with UCRT's `<ctype.h>`.
   - Included `<direct.h>` and undefined Windows macro collisions (`PlaySound`, `LoadImage`).
   - Removed obsolete `_sys_nerr` / `_sys_errlist` macros absent in modern UCRT.
2. **`src/fileops.c`**:
   - Guarded POSIX `<unistd.h>` and `<sys/types.h>` with `#ifndef WIN32`.
   - Included `<direct.h>` for `_mkdir()`.
   - Cast `is_lesson_file` and `alphasort` arguments under `#ifdef BUILD_MINGW32` to satisfy strict GCC 14+ pointer type checking.
3. **`src/network.c` & `src/tuxmathadmin.c`**:
   - Guarded POSIX `<unistd.h>` and `<fcntl.h>` with `#ifndef WIN32`.
4. **`src/setup.c`**:
   - Added missing `T4K_SetScreen(screen);` call right after `SDL_CreateSurface()`. (Previously omitted in the SDL3 transition, causing `T4K_GetScreen()` to return `NULL` and segfaulting in `get_scale()`).

---

## 3. Runtime & Startup Issues Resolved

1. **`libt4k_common.dll` Dynamic Linking**:
   - Rebuilt `t4kcommon` with SDL3 headers and deployed the DLL alongside `TuxMath.exe`.
2. **Missing `libespeak-ng.dll`**:
   - Installed `mingw-w64-ucrt-x86_64-espeak-ng` and `pcaudiolib` in MSYS2, placing runtime DLLs next to the executable.
3. **Phonetic Lookup Table Error (`phontab`)**:
   - Discovered that `T4K_Tts_init()` defaults the espeak path to `.` on Windows.
   - Deployed `espeak-ng-data/` into the build directory, resolving the `exit(1)` abort.
4. **Startup Crash (SIGSEGV in `get_scale()`)**:
   - Traced in `gdb` to a NULL pointer dereference from `T4K_GetScreen()`.
   - Fixed by calling `T4K_SetScreen(screen);` in `src/setup.c`.

---

## 4. Launch & Build Scripts Created

- **`run_tuxmath.bat`** (Root and `build-win/src/`):
  - Convenient launcher that automatically sets MSYS2 UCRT64 `PATH` and starts the game in windowed mode by default.
- **`buildw32/cmake-win.bat`**:
  - Script for rebuilding the project from scratch in an MSYS2 shell.

---

## 5. Verification Status

| Check | Result | Details |
|---|---|---|
| **Compilation** | ✅ Passed | 100% targets built (`TuxMath.exe` — 1.33 MB) |
| **Translations** | ✅ Passed | 36 `.gmo` gettext catalogs compiled |
| **Dependencies** | ✅ Passed | Zero missing DLLs via `ldd` |
| **Process Execution** | ✅ Passed | Process active, rendering window, `Responding: True` |
| **Ubuntu Compatibility** | ✅ Preserved | `configure.ac` and `Makefile.am` remain 100% untouched |
| **Math Command Fleet** | ✅ Working | Verified in runtime log |

---

## 6. Post-Porting Runtime Fixes (Sep 2026)

### Root Cause: `.txt` Extension Appending — `src/fileops.c`

The original `BUILD_MINGW32` code in `read_named_config_file()` unconditionally appended `.txt` to **every** filename before searching for data files. This was legacy behaviour for classic MinGW, where Windows Explorer expected file extensions. In the UCRT64/modern MSYS2 build, **all data files ship without extension** (same layout as Linux), so the `.txt` appending caused every config-file lookup to fail.

**Symptoms fixed:**
- "Could not find global config file." → `read_global_config_file()` was looking for `options.txt` instead of `options`
- "Could not find file: data/missions/lessons/lessonNN" → lesson config files not found
- Arcade / Play Arcade Game not starting → `arcade/space_cadet` etc. not found
- Math Command Training Academy missions not launching → same `.txt` issue

**Fix applied (guarded under `#ifdef BUILD_MINGW32`, does not affect Linux):**

1. `read_global_config_file()`: try `options` first; fall back to `options.txt` only on MinGW (handles both layouts).
2. `read_named_config_file()`: removed the unconditional `.txt` append. The `filename` pointer now equals `fn` directly (no extension mangling), so all location-search attempts (cwd, absolute, missions, lessons, arcade, home-dir) operate on the correct bare filename.

These two changes restore full functionality for:
- ✅ Math Command Training Academy (lessons)
- ✅ Play Arcade Game (all 5 difficulty levels)
- ✅ Play Custom Game
- ✅ Campaign / Math Command Fleet
- ✅ Global options file loading
