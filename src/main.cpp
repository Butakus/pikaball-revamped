#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include "game.hpp"

int main() {
    // Create a new window and load the sprite sheet textures
    pika::Game game;
    game.run();
    return 0;
}
