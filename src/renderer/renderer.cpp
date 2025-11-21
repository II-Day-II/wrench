#include "renderer.h"


namespace Wrench {

    bool Renderer::init(std::shared_ptr<VulkanCtx> &vk_ctx) noexcept
    {
        this->ctx = vk_ctx;
        return init_swapchain();
    }

    void Renderer::render([[maybe_unused]] Scene *scene) noexcept
    {

    }

    void Renderer::cleanup() noexcept
    {
    }

    bool Renderer::init_swapchain()
    {
        return true;
    }

    void Renderer::resize()
    {

    }

}; // namespace Wrench