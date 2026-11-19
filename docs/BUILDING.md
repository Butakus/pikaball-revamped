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

### Install layout for non-embedded builds

For non-embedded builds, assets are installed under the install prefix using:

- `PIKA_ASSETS_INSTALL_DIR` (default: `assets`)

For example:

```bash
cmake -S . -B build/release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DPIKA_EMBED_ASSETS=OFF \
  -DPIKA_ASSETS_INSTALL_DIR=assets
```

### Runtime asset path for package managers

Packaging systems such as Flatpak may need an explicit runtime root for assets. Use:

- `PIKA_ASSETS_RUNTIME_DIR`

This can point to the location where the assets will be available at runtime, for example inside a sandboxed application install.

## Installing the build

A typical install step is:

```bash
cmake --install build/release
```

You can override the install prefix if needed:

```bash
cmake --install build/release --prefix /path/to/stage
```

For package-specific setups, combine the install prefix with the asset-related CMake options above.
