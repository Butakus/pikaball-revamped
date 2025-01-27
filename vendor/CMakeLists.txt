
# Force building a static SDL library
set(SDL_STATIC ON)
add_subdirectory(SDL)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE
    SDL3::SDL3
)
