#include "render_graph.h"

namespace Wrench
{
    bool RenderGraph::init(std::shared_ptr<VulkanCtx> &ctx) noexcept
    {
        m_ctx = ctx;
    }

    void RenderGraph::render() noexcept
    {
        for (auto& node : m_nodes)
        {
            node.run();
        }
    }
};
