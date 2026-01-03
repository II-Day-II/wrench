#include "render_graph.h"

namespace Wrench
{
    bool RenderGraph::init(std::shared_ptr<VulkanCtx> &ctx) noexcept
    {
        m_ctx = ctx;
        return true;
    }

    void RenderGraph::render(VkCommandBuffer cmd, DrawImages &draw_images, std::unique_ptr<Scene> &scene) noexcept
    {
        for (auto& node : m_nodes)
        {
            node.run(cmd, draw_images, scene);
        }
    }
};
