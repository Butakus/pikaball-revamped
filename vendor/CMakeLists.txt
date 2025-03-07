
# Force building a static SDL library
set(SDL_STATIC ON)
set(SDL_SHARED OFF)

# Grab SDL source with FetchContent
include(FetchContent)
FetchContent_Declare(SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         release-3.2.8
)
FetchContent_MakeAvailable(SDL3)


add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE
    SDL3::SDL3
)
