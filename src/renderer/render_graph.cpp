#include "render_graph.h"

namespace Wrench
{
    bool RenderGraph::init(std::shared_ptr<VulkanCtx> &ctx) noexcept
    {
        m_ctx = ctx;
        bool nodes_ok = true;
        for (auto& node : m_nodes)
        {
            nodes_ok &= node->init(ctx);
        }
        return nodes_ok;
    }

    void RenderGraph::render(VkCommandBuffer cmd, DrawImages &draw_images, std::unique_ptr<Scene> &scene) noexcept
    {
        for (auto& node : m_nodes)
        {
            node->run(cmd, draw_images, scene);
        }
    }
    void RenderGraph::set_nodes(std::vector<std::unique_ptr<RenderNode>> &nodes) noexcept
    {
        m_nodes = std::move(nodes);
    }
};
