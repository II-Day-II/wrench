#ifndef APPLICATION_H
#define APPLICATION_H
#include <memory>
#include "SDL3/SDL.h"
#include "../scene/scene.h"
#include "../renderer/renderer.h" 
#include "../vulkan_ctx/vulkan_ctx.h"

namespace Wrench {
    
    struct Statistics {
        float frame_time_ms;
    };


    class Application
    {
        Statistics stats;

        struct SDL_Window* m_window {nullptr};
        std::unique_ptr<Scene> m_scene;
        Renderer m_renderer;
        std::shared_ptr<VulkanCtx> m_ctx;

    public:
        bool init() noexcept;
        void run() noexcept;
        void exit() noexcept;
    };

}; // namespace Wrench
#endif // !APPLICATION_H
