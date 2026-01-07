#ifndef RENDER_NODE_H
#define RENDER_NODE_H
#include "../vk_init.h"
#include "../../vulkan_ctx/vulkan_ctx.h"
#include "../../scene/scene.h"
#include <memory>

namespace Wrench
{
    class RenderNode
    {

    public:
        virtual bool init(std::shared_ptr<VulkanCtx> &ctx) noexcept = 0;
        virtual void run(VkCommandBuffer cmd, DrawImages& draw_images, std::unique_ptr<Scene>& scene) noexcept = 0;
        virtual void cleanup() noexcept = 0;
    };



} // namespace Wrench
#endif // !RENDER_NODE_H
