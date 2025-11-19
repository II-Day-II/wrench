#ifndef VULKAN_CTX_H
#define VULKAN_CTX_H

#include "vulkan/vulkan.h"
#include "SDL3/SDL.h"
#include "vma/vk_mem_alloc.h"
#include "../util/util.h"

namespace Wrench 
{
	struct VulkanCtx 
	{
		VkInstance instance;
		VkPhysicalDevice physical_device;
		VkDevice device;
		VkQueue gfx_queue;
		uint32_t gfx_queue_index;

		VkSurfaceKHR surface;
		VkDebugUtilsMessengerEXT debug_messenger;

		VmaAllocator allocator;
		util::DeletionQueue deletion_queue;
		void cleanup() noexcept;
	};
	/// <summary>
	/// Initialize the vulkan context by creating a VkDevice, 
	/// VkPhysicalDevice and VkInstance, etc.
	/// </summary>
	/// <returns> A VulkanCtx struct containing the core vulkan data.</returns>
	bool init_vulkan(VulkanCtx &ctx, SDL_Window *window) noexcept;
}; // namespace Wrench

#endif // !VULKAN_CTX_H
