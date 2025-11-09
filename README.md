# Pikachu Volley - Revamped Edition
A modern C++ reimplementation of the classic Pikachu Volleyball game using SDL3.

This is the spiritual successor of the "Pikachu Volley - Murcia LAN Party special edition".


## About

The original game was created by "SACHI SOFT / SAWAYAKAN Programmers" and "Satoshi Takenouchi" in 1997.
A few years ago, Kyutae Lee  ([gorisanson](https://github.com/gorisanson)) did some amazing work reverse-engineering the game and porting the original code to Javascript: [Pikachu Volleyball game](https://github.com/gorisanson/pikachu-volleyball).
You can even [play it online](https://gorisanson.github.io/pikachu-volleyball/en/)!

This project is a C++ adaptation of the original game using SDL3, heavily inspired by the Javascript version.


This "Revamped" version features:
- **Cross-platform support** using SDL3. Built and tested on Windows and Linux.
- **Fully self-contained binary** - all resources embedded, no external files needed. Just like the original game!
- **Special sprites** modified by the Murcia LAN Party team in 2009.

## Download
You can download the executable for Windows and Linux from the [Releases](https://github.com/butakus/pikaball-revamped/releases) page.

### Windows:
Download the `pikaball_revamped.exe` file and run it!

### Linux:
There is a Flatpak file available for Linux. After downloading it, you can install it with this command:

```flatpak --user install pikaball_revamped.flatpak```

The flatpak will create a `.desktop` entry for you.
You can launch the game by clicking on the icon, or by running this in the terminal:

```flatpak run io.github.butakus.pikaball```

*Note: You must have flatpak installed with the Flathub repository enabled [[Guide](https://flathub.org/en/setup)].* 

## Features

- Classic Pikachu Volleyball gameplay
- Keyboard controls for player input
- AI opponent for single-player mode
- Original sound effects and music
- Murcia LAN Party 2009 custom sprites (plus some new ones)
- Cross-platform support (Windows, Linux)
- Single portable executable with embedded assets (no installation required)

## Controls

**Left player controls:**
- Movement: **RDFG** keys
- Hit: **Z** or **Space Bar**

**Right player controls:**
- Movement: Arrow keys
- Hit: **Enter** or **Numpad Enter**

**Other:**
- Both left and right player controls can be used to navigate the menus.
- The **Esc** key pauses the game and opens a menu to change the game options (speed, points, and music).
- The **F3** key toggles a small panel that displays current FPS.

*Joystick support is planned for a future version*.

## Build
If you want to build the game yourself, follow the instructions below.

The project uses CMake presets for easy configuration and building across different platforms. All dependencies (SDL3, SDL3_mixer, etc.) are vendored and will be built automatically. All game assets (sprites, sounds, fonts) are embedded directly into the binary during CMake configuration.

### Build Requirements
- **CMake** 3.25 or higher
- **Ninja** build system
- **C++23** compatible compiler:
   - Linux: GCC 13+ or Clang 16+
   - Windows: MinGW-w64 (GCC 13+)

### Linux

1. **Clone the repository:**
   ```bash
   git clone https://github.com/butakus/pikaball-revamped.git
   cd pikaball-revamped
   ```

2. **Configure and build the project:**
   
   For a Release build:
   ```bash
   cmake --preset release
   cmake --build --preset release
   ```
   
   For the Debug build, change the preset to "debug".


The executable will be located in:
- Debug: `build/debug/pikaball_revamped`
- Release: `build/release/pikaball_revamped`

### Windows (MinGW)

1. **Prerequisites:**
   - Install MinGW-w64 with GCC 13 or later
   - Install CMake (3.25+)
   - Install Ninja build system
   - Ensure MinGW's `bin` directory is in your PATH

2. **Clone the repository:**
   ```bash
   git clone https://github.com/butakus/pikaball-revamped.git
   cd pikaball-revamped
   ```

3. **Configure and build the project:**
   ```bash
   cmake --preset windows-release
   cmake --build --preset windows-release
   ```

The executable will be located in `build/release-win/pikaball_revamped.exe`.

**Note:** The Windows executable is fully portable and can be run on any Windows system without installing anything. Just copy the `.exe` file and run it!

## Credits

- **Original Game**: (C) SACHI SOFT / SAWAYAKAN Programmers, 1997 (C) Satoshi Takenouchi
- **JavaScript version, Reverse-Engineered**: [gorisanson](https://github.com/gorisanson/pikachu-volleyball)
- **MLP Sprites**:Murcia LAN Party team (2009)
- **C++ Revamped Version**: Butakus (2025)
