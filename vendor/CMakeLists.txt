include(FetchContent)

# Grab SDL sources with FetchContent
# Force building a static SDL library
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
FetchContent_Declare(SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    # GIT_TAG         release-3.2.20
    GIT_TAG         main
)

set(SDL3_DIR ${SDL3_BINARY_DIR})
FetchContent_Declare(SDL_Mixer
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG         main
    # Last commit before the new SDL3_mixer API.
    # Waiting for SDL3_Mixer actual release (with SDL 3.4.0)
    # GIT_TAG 4e9a308a0daa63b6bd492b0ec616f86337500898
)

FetchContent_MakeAvailable(SDL3 SDL_mixer)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
)
