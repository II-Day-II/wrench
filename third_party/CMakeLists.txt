# add third party directories (git submodules)
set(SDL_TEST OFF)
add_subdirectory(SDL)

add_subdirectory(glm)

add_subdirectory(vk-bootstrap)

add_subdirectory(VulkanMemoryAllocator)


# imgui doesn't have a cmakelists so we do that stuff here
add_library(imgui INTERFACE)
target_include_directories(imgui INTERFACE imgui)
target_sources(imgui INTERFACE
    imgui/imconfig.h
    imgui/imgui_internal.h

    imgui/imgui.h
    imgui/imgui.cpp
    
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h

    imgui/backends/imgui_impl_sdl3.h
    imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_vulkan.h
    imgui/backends/imgui_impl_vulkan.cpp
)
find_package(Vulkan REQUIRED)
target_link_libraries(imgui INTERFACE 
    SDL3::SDL3
    Vulkan::Vulkan
)

set(SLANG_ENABLE_RHI OFF)
set(SLANG_ENABLE_GFX OFF)
set(SLANG_ENABLE_TESTS OFF)
set(SLANG_ENABLE_OPTIX OFF)
set(SLANG_ENABLE_NVAPI OFF)
set(SLANG_ENABLE_CUDA OFF)
set(SLANG_ENABLE_SLANG_GLSLANG OFF)
set(SLANG_ENABLE_EXAMPLES OFF)
set(SLANG_EXCLUDE_DAWN ON)
set(SLANG_EXCLUDE_TINT ON)
set(SLANG_OVERRIDE_IMGUI_PATH ".")
# add_subdirectory(slang)

# add interface library that the main cmakelists can link against to include all these
add_library(third_party INTERFACE)
target_link_libraries(third_party INTERFACE 
    SDL3::SDL3
    glm::glm
    vk-bootstrap::vk-bootstrap
    GPUOpen::VulkanMemoryAllocator
    # slang::slang
    imgui
)

