#include <iostream>
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include "window.hpp"

// Path to image that will be loaded
// const std::string image_path = "assets/hello-sdl3.bmp";
const std::string image_path = "assets/images/sprite_sheet.bmp";

Window::SDL_Surface_ptr load_image() {
    // Load splash image from disk
    return {SDL_LoadBMP(image_path.c_str()), SDL_DestroySurface};
}

int main() {
    // Create a new window and load the sprite sheet textures
    Window window;

    SDL_Log("Running stuff!");
    bool quit {false};

    // Event data
    SDL_Event event;
    SDL_zero(event);  // Actually not required?

    while (!quit) {
        // PollEvent checks new events to process. Not thread safe.
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // Update surface content
        window.render();
    }

    SDL_Log("SDL out!");
    return 0;

}
