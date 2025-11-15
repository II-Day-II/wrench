#ifndef RENDERER_H
#define RENDERER_H
#include "../scene/scene.h"
#include "SDL3/SDL.h"

namespace Wrench {

    class Renderer
    {
    public:
        bool init(SDL_Window *window) noexcept;
        void render(Scene *scene) noexcept;
        void cleanup() noexcept;
    };

}; // namespace Wrench
#endif // !RENDERER_H
