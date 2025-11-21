# Pikachu Volley - Revamped Edition
Una reimplementación moderna en C++ del clásico juego *Pikachu Volleyball* usando SDL3.

Este es el sucesor espiritual del "Pikachu Volley - Murcia LAN Party special edition".

## Acerca de

El juego original fue creado por "SACHI SOFT / SAWAYAKAN Programmers" y "Satoshi Takenouchi" en 1997.
Hace unos años, Kyutae Lee ([gorisanson](https://github.com/gorisanson)) hizo un trabajo increíble aplicando ingeniería inversa al juego y portando el código original a Javascript: [Pikachu Volleyball game](https://github.com/gorisanson/pikachu-volleyball).
¡Incluso puedes [jugarlo online](https://gorisanson.github.io/pikachu-volleyball/en/)!

Este proyecto es una adaptación en C++ del juego original usando SDL3, fuertemente inspirado en la versión de Javascript.

Esta versión "Revamped" incluye:
* **Soporte multiplataforma** usando SDL3. Compilado y probado en Windows y Linux.
* **Ejecutable 100% portable**: todos los recursos están integrados, no se necesitan archivos externos. ¡Igual que el juego original!
* **Sprites especiales** modificados por el equipo de la Murcia LAN Party en 2009.

## Descarga

Puedes descargar el ejecutable para Windows y Linux desde la página de [Releases](https://github.com/butakus/pikaball-revamped/releases).

### Windows:

¡Descarga el archivo `pikaball_revamped.exe` y ejecútalo!

### Linux:

Hay un archivo Flatpak disponible para Linux. Después de descargarlo, puedes instalarlo con este comando:

```bash
flatpak --user install pikaball_revamped.flatpak
```

El flatpak creará una entrada `.desktop`.
Puedes iniciar el juego haciendo clic en el icono o ejecutando esto en la terminal:

```bash
flatpak run io.github.butakus.pikaball
```

*Nota: Debes tener flatpak instalado con el repositorio Flathub configurado [[Guía](https://flathub.org/en/setup)].*

## Características

* Jugabilidad clásica del *Pikachu Volleyball*.
* Control de los jugadores por teclado.
* Modo de un solo jugador contra oponente IA.
* Efectos de sonido y música originales.
* Sprites personalizados de la Murcia LAN Party 2009 (además de algunos nuevos).
* Soporte multiplataforma (Windows, Linux).
* Ejecutable único y portable con recursos integrados (no requiere instalación).

## Controles

**Controles del jugador izquierdo:**

* Movimiento: Teclas **RDFG**
* Hit: **Z** o **Barra espaciadora**

**Controles del jugador derecho:**

* Movimiento: Flechas de dirección
* Hit: **Enter** o **Enter del teclado numérico**

**Otros:**

* Los controles de ambos jugadores (izquierdo y derecho) se pueden usar para navegar por los menús.
* La tecla **Esc** pausa el juego y abre un menú para cambiar las opciones del juego (velocidad, puntos y música).
* La tecla **F3** alterna un pequeño panel que muestra los FPS actuales.

*El soporte para joystick está planeado para una versión futura.*

## Compilación (Build)

Si deseas compilar el juego desde fuente, sigue las instrucciones que aparecen a continuación.

El proyecto utiliza *presets* de CMake para facilitar la configuración y compilación en diferentes plataformas.
Todas las dependencias (SDL3, SDL3_mixer, etc.) están incluidas (*vendored*) y se compilarán automáticamente.
Todos los recursos del juego (sprites, sonidos, fuentes) se integran directamente en el binario durante la configuración de CMake.

### Requisitos de compilación

* **CMake** 3.25 o superior
* Sistema de compilación **Ninja**
* Compilador compatible con **C++23**:
    * Linux: GCC 13+ o Clang 16+
    * Windows: MinGW-w64 (GCC 13+)

### Linux

1.  **Clona el repositorio:**

    ```bash
    git clone https://github.com/butakus/pikaball-revamped.git
    cd pikaball-revamped
    ```

2.  **Configura y compila el proyecto:**

    Para compilar en "Release":

    ```bash
    cmake --preset release
    cmake --build --preset release
    ```

    Para compilar en "Debug", cambia el preset a "debug".

El ejecutable se creará en:

* Debug: `build/debug/pikaball_revamped`
* Release: `build/release/pikaball_revamped`

### Windows (MinGW)

1.  **Prerrequisitos:**

    * Instalar MinGW-w64 con GCC 13 o posterior.
    * Instalar CMake (3.25+).
    * Instalar Ninja (el sistema de compilación).
    * Asegurarse de que el directorio `bin` de MinGW esté en el PATH.

2.  **Clonar el repositorio:**

    ```bash
    git clone https://github.com/butakus/pikaball-revamped.git
    cd pikaball-revamped
    ```

3.  **Configurar y compilar el proyecto:**

    ```bash
    cmake --preset windows-release
    cmake --build --preset windows-release
    ```

El ejecutable se creará en `build/release-win/pikaball_revamped.exe`.

**Nota:** El ejecutable de Windows es totalmente portable y puede ejecutarse en cualquier sistema Windows sin instalar nada.
Simplemente, copia el archivo `.exe` y ejecútalo.

## Créditos

* **Juego Original**: (C) SACHI SOFT / SAWAYAKAN Programmers, 1997 (C) Satoshi Takenouchi
* **Versión JavaScript**: [gorisanson](https://github.com/gorisanson/pikachu-volleyball)
* **Sprites MLP**: Equipo de Murcia LAN Party (2009)
* **Versión "Revamped" en C++**: Butakus (2025)
