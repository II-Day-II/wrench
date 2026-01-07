#include "hello_triangle.h"
#include "../../vulkan_ctx/vulkan_ctx.h"
#include "../../scene/scene.h"

namespace Wrench
{
	bool HelloTriangle::init([[maybe_unused]] std::shared_ptr<VulkanCtx> &ctx) noexcept
	{
		// init pipeline
		return true;
	}

	void HelloTriangle::cleanup() noexcept
	{

	}

	void HelloTriangle::run(VkCommandBuffer cmd, DrawImages& draw_images, [[maybe_unused]] std::unique_ptr<Scene>& scene) noexcept
	{
		VkExtent2D draw_extent = { draw_images.draw_image.extent.width, draw_images.draw_image.extent.height };
		
		VkClearValue clear_color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		VkRenderingAttachmentInfo color_attachment = vkinit::attachment_info(draw_images.draw_image.view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		
		VkRenderingAttachmentInfo depth_attachment = vkinit::depth_attachment_info(draw_images.depth_image.view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
		
		VkRenderingInfo render_info = vkinit::rendering_info(draw_extent, &color_attachment, &depth_attachment);

		vkCmdBeginRendering(cmd, &render_info);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.pipeline);

		set_viewport_and_scissor(cmd, draw_extent);

		vkCmdDraw(cmd, 3, 1, 0, 0);

		vkCmdEndRendering(cmd);
	}
}