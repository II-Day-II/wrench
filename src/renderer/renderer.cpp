#include "renderer.h"


namespace Wrench {

    bool Renderer::init(SDL_Window *window) noexcept
    {
        
        return true && window != nullptr;
    }

    void Renderer::render([[maybe_unused]] Scene *scene) noexcept
    {

    }

    void Renderer::cleanup() noexcept
    {
    }

}; // namespace Wrench