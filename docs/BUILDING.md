# Building Pikachu Volley - Revamped Edition

This document contains the detailed build instructions for the project.

## Requirements

- CMake 3.25 or newer
- Ninja build system
- A C++23-capable compiler
  - Linux: GCC 13+ or Clang 16+
  - Windows: MinGW-w64 with GCC 13+

MSVC is not supported: embedded assets are compiled as very large string literals, beyond MSVC's limits,
and Windows builds link statically with MinGW's `-static` flag. CMake prints a warning if MSVC is detected.

## Build presets

The project uses CMake presets for the most common workflows:

- `debug`: non-embedded build for debugging
- `release`: non-embedded release build
- `release-embedded`: embedded assets release for a portable build
- `windows-release`: Windows non-embedded release build
- `windows-release-embedded`: Windows embedded assets release for a portable build

## Linux build

1. Clone the repository:

   ```bash
   git clone https://github.com/butakus/pikaball-revamped.git
   cd pikaball-revamped
   ```

2. Configure and build the default non-embedded release build:

   ```bash
   cmake --preset release
   cmake --build --preset release
   ```

3. Configure and build the embedded release build:

   ```bash
   cmake --preset release-embedded
   cmake --build --preset release-embedded
   ```

4. For a debug build, use:

   ```bash
   cmake --preset debug
   cmake --build --preset debug
   ```

The resulting binaries are placed under the build directories created by the presets.
Non-embedded builds also get an `assets` symlink next to the binary (a copy on Windows), so the game can be run
straight from the build directory, e.g. `./build/debug/pikaball_revamped`.

### Compiler warnings

The project's own code is compiled with `-Wall -Wextra -Wpedantic` (dependencies such as SDL are not affected).
To treat these warnings as errors, configure with `-DPIKA_WERROR=ON`.
The CI Debug builds use this option, so new warnings make CI fail:

```bash
cmake --preset debug -DPIKA_WERROR=ON
cmake --build --preset debug
```

## Windows build (MinGW)

1. Install the required tools:
   - MinGW-w64 with GCC 13+
   - CMake 3.25+
   - Ninja

2. Clone the repository and enter the project directory.

3. Configure and build the default non-embedded release build:

   ```bash
   cmake --preset windows-release
   cmake --build --preset windows-release
   ```

4. Configure and build the embedded release build:

   ```bash
   cmake --preset windows-release-embedded
   cmake --build --preset windows-release-embedded
   ```

## Asset modes

The project supports two ways to include assets:

- Embedded mode: assets are compiled into the binary. Just like in the original game, it allows to easily move around the executable.
  Changing an embedded asset re-runs the CMake configure step automatically on the next build.
- Non-embedded mode (default): assets are installed in a directory next to the executable. This option is better if you want to play around and change some of the assets!

You can toggle the mode with:

- `PIKA_EMBED_ASSETS=OFF` for non-embedded builds
- `PIKA_EMBED_ASSETS=ON` for embedded builds

## Installing the build

A typical install step is:

```bash
cmake --install build/release
```

By default, the game is installed inside the build directory, in `pikaball-release/` (with the presets, `build/<preset>/pikaball-release/`).
You can choose another location with `--prefix`:

```bash
cmake --install build/release --prefix /path/to/stage
```

### Install layouts

- **Portable (default)**: the executable and the `assets/` directory side by side, like in the build directory and in the Windows release zip.
  The whole directory can be moved around.
- **Unix layout** (`-DPIKA_SYSTEM_INSTALL=ON`): `bin/pikaball_revamped` and `share/pikaball_revamped/assets/`.
  On Linux, it also installs the desktop entry, the AppStream metainfo and the icon (from `dist/linux/`) into `share/`.
  This is the layout used by the Flatpak.

The game first looks for `assets/` next to the executable, and then (Unix layout only) in `../share/pikaball_revamped/` relative to it.

## Flatpak

The Flatpak manifest and a build script are in `dist/flatpak/`.
To build the `pikaball.flatpak` bundle locally (it needs `flatpak-builder` and the Flathub repository):

```bash
dist/flatpak/build_flatpak.sh            # Build dist/flatpak/pikaball.flatpak
dist/flatpak/build_flatpak.sh --install  # Also install it for the current user
```

The CI also builds the bundle on every push to `main` (workflow `Flatpak build`), and uploads it as an artifact.
