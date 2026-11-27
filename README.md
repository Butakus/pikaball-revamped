# Pikachu Volley - Revamped Edition
**English** | [Español (Spanish)](./README_ES.md)

---

A modern C++ reimplementation of the classic Pikachu Volleyball game using SDL3.

This is the spiritual successor of the "Pikachu Volley - Murcia LAN Party special edition".


## About

The original game was created by "SACHI SOFT / SAWAYAKAN Programmers" and "Satoshi Takenouchi" in 1997.
A few years ago, Kyutae Lee  ([gorisanson](https://github.com/gorisanson)) did some amazing work reverse-engineering the game and porting the original code to Javascript: [Pikachu Volleyball game](https://github.com/gorisanson/pikachu-volleyball).
You can even [play it online](https://gorisanson.github.io/pikachu-volleyball/en/)!

This project is a C++ adaptation of the original game using SDL3, heavily inspired by the Javascript version.


This "Revamped" version features:
- **Cross-platform support** using SDL3. Built and tested on Windows and Linux.
- **Two build options**: a portable build with embedded assets (easy to move around, just like the original game) and the default build with assets in a directory (easier to mod and experiment with the game files).
- **Special sprites** modified by the Murcia LAN Party team in 2009.

## Download
You can download the game for Windows and Linux from the [Releases](https://github.com/butakus/pikaball-revamped/releases) page.

### Windows:
Download the `pikaball_revamped.exe` file and run it!

### Linux:
There is a Flatpak file available for Linux. After downloading it, you can install it with this command:

```flatpak --user install pikaball.flatpak```

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
- Two build options available: portable embedded-assets builds and the default directory-based assets build for easier modding

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
If you want to build the game yourself, follow the instructions in [docs/BUILDING.md](docs/BUILDING.md).

## Credits

- **Original Game**: (C) SACHI SOFT / SAWAYAKAN Programmers, 1997 (C) Satoshi Takenouchi
- **JavaScript version, Reverse-Engineered**: [gorisanson](https://github.com/gorisanson/pikachu-volleyball)
- **MLP Sprites**:Murcia LAN Party team (2009)
- **C++ Revamped Version**: Butakus (2025)
