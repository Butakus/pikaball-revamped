include(FetchContent)

# Grab SDL sources with FetchContent
# Force building a static SDL library
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
set(BUILD_SHARED_LIBS OFF)
set(SDL_DEPS_SHARED ON)
set(SDLMIXER_DEPS_SHARED ON)
set(SDLMIXER_VENDORED OFF)
set(SDLMIXER_SAMPLES OFF)
set(SDLTTF_VENDORED ON)
# TODO: Adjust SDL compilation options to reduce binary size
# GIT_SHALLOW: clone only the tagged commit, not the whole history
FetchContent_Declare(SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         release-3.4.12
    GIT_SHALLOW     TRUE
    SYSTEM  # Headers are system includes: project warning flags (and -Werror) don't apply to them
)
FetchContent_Declare(SDL_mixer
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG         release-3.2.4
    GIT_SHALLOW     TRUE
    SYSTEM  # Headers are system includes: project warning flags (and -Werror) don't apply to them
    # The vendored codec libraries (external/ submodules) are only used with SDLMIXER_VENDORED, which is OFF
    GIT_SUBMODULES  ""
)
FetchContent_Declare(SDL_ttf
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG         release-3.2.2
    GIT_SHALLOW     TRUE
    SYSTEM  # Headers are system includes: project warning flags (and -Werror) don't apply to them
)

FetchContent_MakeAvailable(SDL3 SDL_mixer SDL_ttf)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE
    SDL3::SDL3-static
    SDL3_mixer::SDL3_mixer-static
    SDL3_ttf::SDL3_ttf-static
)
