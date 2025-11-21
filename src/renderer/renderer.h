#ifndef RENDERER_H
#define RENDERER_H
#include <memory>
#include "../scene/scene.h"
#include "../vulkan_ctx/vulkan_ctx.h"
#include "SDL3/SDL.h"

namespace Wrench {

    class Renderer
    {
        std::shared_ptr<VulkanCtx> ctx;
    public:
        bool init(std::shared_ptr<VulkanCtx> &vk_ctx) noexcept;
        void render(Scene *scene) noexcept;
        void resize();
        void cleanup() noexcept;
    private:
        bool init_swapchain();
    };

}; // namespace Wrench
#endif // !RENDERER_H
