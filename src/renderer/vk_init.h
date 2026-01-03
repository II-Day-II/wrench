#ifndef VK_INIT_H
#define VK_INIT_H
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

namespace Wrench
{
	struct AllocatedImage
	{
		VkImage image;
		VkImageView view;
		VmaAllocation allocation;
		VkExtent3D extent;
		VkFormat format;
	};

	struct AllocatedBuffer
	{
		VkBuffer buffer;
		VmaAllocation allocation;
		VmaAllocationInfo allocation_info;
	};



	namespace vkinit {
		//> init_cmd
		VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
		VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count = 1);
		//< init_cmd

		VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0);
		VkCommandBufferSubmitInfo command_buffer_submit_info(VkCommandBuffer cmd);

		VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);

		VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags = 0);

		VkSubmitInfo2 submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo,
			VkSemaphoreSubmitInfo* waitSemaphoreInfo);
		VkPresentInfoKHR present_info();

		VkRenderingAttachmentInfo attachment_info(VkImageView view, VkClearValue* clear, VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

		VkRenderingAttachmentInfo depth_attachment_info(VkImageView view,
			VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

		VkRenderingInfo rendering_info(VkExtent2D renderExtent, VkRenderingAttachmentInfo* colorAttachment,
			VkRenderingAttachmentInfo* depthAttachment);

		VkImageSubresourceRange image_subresource_range(VkImageAspectFlags aspectMask);

		VkSemaphoreSubmitInfo semaphore_submit_info(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
		VkDescriptorSetLayoutBinding descriptorset_layout_binding(VkDescriptorType type, VkShaderStageFlags stageFlags,
			uint32_t binding);
		VkDescriptorSetLayoutCreateInfo descriptorset_layout_create_info(VkDescriptorSetLayoutBinding* bindings,
			uint32_t bindingCount);
		VkWriteDescriptorSet write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet,
			VkDescriptorImageInfo* imageInfo, uint32_t binding);
		VkWriteDescriptorSet write_descriptor_buffer(VkDescriptorType type, VkDescriptorSet dstSet,
			VkDescriptorBufferInfo* bufferInfo, uint32_t binding);
		VkDescriptorBufferInfo buffer_info(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);

		VkImageCreateInfo image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
		VkImageViewCreateInfo imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
		VkPipelineLayoutCreateInfo pipeline_layout_create_info();
		VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage,
			VkShaderModule shaderModule,
			const char* entry = "main");
	} // namespace vkinit

	namespace vkutil
	{
		/// <summary>
		/// Transition an image from one layout to another by inserting a memory barrier. 
		/// You must track its current layout yourself.
		/// </summary>
		/// <param name="cmd"></param>
		/// <param name="image"></param>
		/// <param name="currentLayout"></param>
		/// <param name="newLayout"></param>
		/// <param name="force_depth"> true if this barrier should force the depth aspect to be transitioned, despite not transitioning to depth attachment optimal</param>
		void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, bool force_depth = false);
		/// <summary>
		/// Copy one image to another via blitting. Only accounts for mip 0.
		/// </summary>
		/// <param name="cmd"></param>
		/// <param name="source"></param>
		/// <param name="destination"></param>
		/// <param name="srcSize"></param>
		/// <param name="dstSize"></param>
		void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);
		/// <summary>
		/// Generate mipmaps for an image.
		/// </summary>
		/// <param name="cmd"></param>
		/// <param name="image"></param>
		/// <param name="imageSize"></param>
		void generate_mipmaps(VkCommandBuffer cmd, VkImage image, VkExtent2D imageSize);
	} // namespace vkutil
}; // namespace Wrench

#endif // !VK_INIT_H
