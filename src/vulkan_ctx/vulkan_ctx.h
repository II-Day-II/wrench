#ifndef VULKAN_CTX_H
#define VULKAN_CTX_H

#include "vulkan/vulkan.h"

namespace Wrench 
{
	struct VulkanCtx 
	{
		VkInstance instance;
		VkDevice device;
		VkPhysicalDevice physical_device;
		VkQueue gfx_queue;
		VkSwapchainKHR swapchain;
	};
	/// <summary>
	/// Initialize the vulkan context by creating a VkDevice, 
	/// VkPhysicalDevice and VkInstance, etc.
	/// </summary>
	/// <returns> A VulkanCtx struct containing the core vulkan data.</returns>
	bool init_vulkan(VulkanCtx &ctx);

}; // namespace Wrench

#endif // !VULKAN_CTX_H
