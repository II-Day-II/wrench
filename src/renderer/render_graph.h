#ifndef RENDER_GRAPH_H
#define RENDER_GRAPH_H

#include <memory>
#include <vector>
#include "../vulkan_ctx/vulkan_ctx.h"
#include "../scene/scene.h"
#include "vk_init.h"

namespace Wrench
{
    // defined here to avoid circular include in renderer.h
    struct DrawImages
    {
        AllocatedImage draw_image;
        AllocatedImage depth_image;
    };

    class RenderNode
    {
    public:
        virtual bool init() noexcept = 0;
        virtual void run(VkCommandBuffer cmd, DrawImages &draw_images, std::unique_ptr<Scene> &scene) noexcept = 0;
    };
    
    class RenderGraph
    {
        std::shared_ptr<VulkanCtx> m_ctx;
        std::vector<RenderNode> m_nodes;
    public:
        bool init(std::shared_ptr<VulkanCtx> &ctx) noexcept;
        void render(VkCommandBuffer cmd, DrawImages &draw_images, std::unique_ptr<Scene> &scene) noexcept;
    };

}

#endif // !RENDER_GRAPH_H
