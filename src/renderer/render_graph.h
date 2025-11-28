#ifndef RENDER_GRAPH_H
#define RENDER_GRAPH_H

#include <memory>
#include <vector>
#include "../vulkan_ctx/vulkan_ctx.h"

namespace Wrench
{
    class RenderNode
    {
    public:
        virtual bool init() noexcept = 0;
        virtual void run() noexcept = 0;
    };
    
    class RenderGraph
    {
        std::shared_ptr<VulkanCtx> m_ctx;
        std::vector<RenderNode> m_nodes;
    public:
        bool init(std::shared_ptr<VulkanCtx> &ctx) noexcept;
        void render() noexcept;
    };

}

#endif // !RENDER_GRAPH_H
