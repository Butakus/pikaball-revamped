#include "SDL3/SDL_main.h"

#include "game.hpp"

int main(int argc, char **argv) {
    pika::Game game;
    game.run();
    return 0;
}
