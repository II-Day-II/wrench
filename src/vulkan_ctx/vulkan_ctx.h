#ifndef VULKAN_CTX_H
#define VULKAN_CTX_H

#include "vulkan/vulkan.h"
#include "SDL3/SDL.h"
#include "vk_mem_alloc.h"
#include "../util/util.h"

namespace Wrench 
{
	struct VulkanCtx 
	{
		VkInstance instance = VK_NULL_HANDLE;
		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue gfx_queue = VK_NULL_HANDLE;
		uint32_t gfx_queue_index{};

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

		VmaAllocator allocator = VK_NULL_HANDLE;
		util::DeletionQueue deletion_queue;

		SDL_Window* window = nullptr;

		bool initialized = false;

		void cleanup() noexcept;

		VulkanCtx(SDL_Window* window) noexcept;
		VulkanCtx(const VulkanCtx&) = delete;
		VulkanCtx(VulkanCtx&&) = delete;
	};
}; // namespace Wrench

#endif // !VULKAN_CTX_H
