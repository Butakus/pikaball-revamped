# Pikachu Volley - Revamped Edition
[English](./README.md) | **Español (Spanish)**

---

Una reimplementación moderna en C++ del clásico juego *Pikachu Volleyball* usando SDL3.

Este es el sucesor espiritual del "Pikachu Volley - Murcia LAN Party special edition".

## Acerca de

El juego original fue creado por "SACHI SOFT / SAWAYAKAN Programmers" y "Satoshi Takenouchi" en 1997.
Hace unos años, Kyutae Lee ([gorisanson](https://github.com/gorisanson)) hizo un trabajo increíble aplicando ingeniería inversa al juego y portando el código original a Javascript: [Pikachu Volleyball game](https://github.com/gorisanson/pikachu-volleyball).
¡Incluso puedes [jugarlo online](https://gorisanson.github.io/pikachu-volleyball/en/)!

Este proyecto es una adaptación en C++ del juego original usando SDL3, fuertemente inspirado en la versión de Javascript.

Esta versión "Revamped" incluye:
* **Soporte multiplataforma** usando SDL3. Compilado y probado en Windows y Linux.
* **Dos opciones de compilación**: una build portable con recursos integrados en el ejecutable (fácil de mover, igual que el juego original) y la versión por defecto con los recursos en un directorio aparte (permite modificar sprites y sonidos).
* **Sprites especiales** modificados por el equipo de la Murcia LAN Party en 2009.

## Descarga

Puedes descargar el juego para Windows y Linux desde la página de [Releases](https://github.com/butakus/pikaball-revamped/releases).

### Windows:

¡Descarga el archivo `pikaball_revamped.exe` y ejecútalo!

### Linux:

Hay un archivo Flatpak disponible para Linux. Después de descargarlo, puedes instalarlo con este comando:

```bash
flatpak --user install pikaball.flatpak
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
* Dos opciones de build disponibles: Ejecutable portable con recursos integrados y la versión por defecto con los recursos en un directorio aparte para poder modificarlos.

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
Si quieres compilar el juego desde fuente, sigue las instrucciones que aparecen en [docs/BUILDING.md](docs/BUILDING.md).

## Créditos

* **Juego Original**: (C) SACHI SOFT / SAWAYAKAN Programmers, 1997 (C) Satoshi Takenouchi
* **Versión JavaScript**: [gorisanson](https://github.com/gorisanson/pikachu-volleyball)
* **Sprites MLP**: Equipo de Murcia LAN Party (2009)
* **Versión "Revamped" en C++**: Butakus (2025)
