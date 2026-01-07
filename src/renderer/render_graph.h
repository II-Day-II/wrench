#ifndef RENDER_GRAPH_H
#define RENDER_GRAPH_H

#include <memory>
#include <vector>
#include "../vulkan_ctx/vulkan_ctx.h"
#include "../scene/scene.h"
#include "vk_init.h"
#include "render_graph_nodes/render_node.h"


namespace Wrench
{
    class RenderGraph
    {

        std::shared_ptr<VulkanCtx> m_ctx;
        std::vector<std::unique_ptr<RenderNode>> m_nodes;
    public:


        bool init(std::shared_ptr<VulkanCtx> &ctx) noexcept;
        void render(VkCommandBuffer cmd, DrawImages &draw_images, std::unique_ptr<Scene> &scene) noexcept;
        void set_nodes(std::vector<std::unique_ptr<RenderNode>> &nodes) noexcept;
    };

}

#endif // !RENDER_GRAPH_H
