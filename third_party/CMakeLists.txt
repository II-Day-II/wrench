# TODO: Add SDL, GLM, VMA etc
set(SDL_TEST OFF)
add_subdirectory(SDL)

add_library(third_party INTERFACE)
target_link_libraries(third_party INTERFACE 
    SDL3::SDL3
)
