#ifndef HELLO_TRIANGLE_H
#define HELLO_TRAINGLE_H

#include "render_node.h"
#include "../pipeline.h"

namespace Wrench
{
	class HelloTriangle : public RenderNode
	{
		Pipeline m_pipeline{};
		bool init(std::shared_ptr<VulkanCtx> &ctx) noexcept override;
		void cleanup() noexcept override;
		void run(VkCommandBuffer cmd, DrawImages &draw_images, std::unique_ptr<Scene> &scene) noexcept override;
	};
}

#endif // !HELLO_TRIANGLE_H
