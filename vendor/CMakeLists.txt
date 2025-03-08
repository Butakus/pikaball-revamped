include(FetchContent)

# Grab SDL sources with FetchContent
# Force building a static SDL library
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
FetchContent_Declare(SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         release-3.2.8
)

set(SDL3_DIR ${SDL3_BINARY_DIR})
FetchContent_Declare(SDL_Mixer
        GIT_REPOSITORY  https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG         main
)

FetchContent_MakeAvailable(SDL3 SDL_mixer)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
)
