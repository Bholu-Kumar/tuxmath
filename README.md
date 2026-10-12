# TuxMath (SDL3 Edition)

**TuxMath** (Tux, of Math Command) is an educational arcade game starring Tux, the Linux Penguin. This repository has been migrated to **SDL3** and updated with accessibility features (Text-To-Speech navigation, dynamic resolution scaling, and localized audio feedback).

---

## Project Team & Organization

This SDL3 edition of TuxMath is developed under **[Zendalona](https://zendalona.com/)**.

| Role | Name | Contact |
|---|---|---|
| **Founder, Zendalona** | Nalin Sathyan | [Nalin.x.Linux@gmail.com](mailto:Nalin.x.Linux@gmail.com) |
| **Mentor** | Nalin Sathyan | — |
| **Mentor** | Mukundhan Annamalai | — |
| **Mentor** | Deepak Aggarwal | [deepak.aggarwal9@gmail.com](mailto:deepak.aggarwal9@gmail.com) |
| **SDL3 Migration & Lead Developer** | Midhun M | [mmidhun781@gmail.com](mailto:mmidhun781@gmail.com) |
| **SDL3 Migration Co-Developer** | Bholu Kumar D | [bholukumar3352@gmail.com](mailto:bholukumar3352@gmail.com) |

### Key Contributions by Midhun M & Bholu Kumar D
- **SDL 1.2 → SDL3 migration**: Complete port of the entire codebase from SDL 1.2 to SDL3
- **F5 — TTS Toggle**: Implementation of runtime Text-To-Speech toggle (F5 key)
- **Windows build**: Native Windows MSIX package and CMake build system
- **macOS build**: macOS app bundle and build system
- **Ubuntu build**: Linux CMake build system and CI workflow
- **t4kcommon integration**: Dynamic resolution scaling and accessibility integration

> Mentored by **Nalin Sathyan** (Founder, Zendalona), **Mukundhan Annamalai**, and **Deepak Aggarwal**, under the **Zendalona** organization.

---

## Prerequisites

Before compiling TuxMath, make sure `t4kcommon` (SDL3 version) and development libraries are installed on your system.

```bash
# Core Dependencies (Debian / Ubuntu / Linux Mint)
sudo apt-get update
sudo apt-get install -y build-essential autoconf automake libtool pkg-config libxml2-dev librsvg2-dev libpng-dev libespeak-ng-dev
```

### Install t4kcommon First
```bash
git clone https://github.com/Midhun-M-git/t4kcommon.git
cd t4kcommon
mkdir -p build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
sudo make install
sudo ldconfig
```

---

## Build & Installation Guide (Linux / Autotools)

### Step 1: Clone the Repository
```bash
git clone https://github.com/Midhun-M-git/tuxmath.git
cd tuxmath
```

### Step 2: Build with Autotools
```bash
# Generate build configuration files
./autogen.sh

# Configure TuxMath
./configure

# Build executable
make -j$(nproc)
```

### Step 3: Install
```bash
sudo make install
```

---

## Build & Installation Guide (Windows / CMake)

TuxMath supports native Windows builds using **MSYS2 (UCRT64)** and **CMake**.

### Step 1: Prerequisites in MSYS2 UCRT64
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-cmake \
          mingw-w64-ucrt-x86_64-pkgconf \
          mingw-w64-ucrt-x86_64-sdl3 \
          mingw-w64-ucrt-x86_64-sdl3-image \
          mingw-w64-ucrt-x86_64-sdl3-mixer \
          mingw-w64-ucrt-x86_64-sdl3-ttf \
          mingw-w64-ucrt-x86_64-sdl3-net \
          mingw-w64-ucrt-x86_64-librsvg \
          mingw-w64-ucrt-x86_64-libxml2 \
          mingw-w64-ucrt-x86_64-gettext \
          make
```

### Step 2: Build `t4kcommon`
```bash
git clone https://github.com/Midhun-M-git/t4kcommon.git
cd t4kcommon
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

### Step 3: Build `tuxmath`
```bash
git clone https://github.com/Midhun-M-git/tuxmath.git
cd tuxmath
mkdir build-win && cd build-win
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

---

## Key Shortcuts & Accessibility Controls

| Key | Description |
|---|---|
| **F5** | Toggle Text-To-Speech (TTS) On / Off |
| **ESC** | Pause game or Return to Previous Menu |
| **Space** | Continue / Skip dialogs |

---

## Features & Game Modes
- **Math Command Academy**: Defend your igloos by answering math problems (addition, subtraction, multiplication, division).
- **Factoroids**: Factorize composite numbers and simplify fractions with your laser cannon!
- **Play Arcade & Campaign**: Journey through progressively challenging math missions.
- **Multiplayer & LAN**: Compete with friends over the local network.
- **Accessibility Integration**: Real-time TTS screen reading, debounced key handling, and high-resolution display scaling.
