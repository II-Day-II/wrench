#ifndef VULKAN_CTX_H
#define VULKAN_CTX_H

#include <iostream>
#include "vulkan/vulkan.h"
#include "vulkan/vk_enum_string_helper.h"
#include "SDL3/SDL.h"
#include "vk_mem_alloc.h"
#include "../util/util.h"

#define VK_CHECK_MACRO(x)   \
	do {					\
		VkResult err = x;	\
		if (err) {          \
			std::cout << "Vulkan error: [" << string_VkResult(err) << "]" << std::endl;\
			if (err < 0)    \
				abort();    \
		}                   \
	} while (false);

namespace Wrench 
{
	void vk_check_fn(VkResult res);
	
	struct VulkanCtx 
	{
		VkInstance instance = VK_NULL_HANDLE;
		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue gfx_queue = VK_NULL_HANDLE;
		uint32_t gfx_queue_index{};

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

		VkCommandPool immediate_command_pool = VK_NULL_HANDLE;
		VkCommandBuffer immediate_command_buffer = VK_NULL_HANDLE;
		VkFence immediate_fence = VK_NULL_HANDLE;

		VmaAllocator allocator = VK_NULL_HANDLE;
		util::DeletionQueue deletion_queue;

		SDL_Window* window = nullptr;

		bool initialized = false;

		void cleanup() noexcept;

		VulkanCtx(SDL_Window* window) noexcept;
		VulkanCtx(const VulkanCtx&) = delete;
		VulkanCtx(VulkanCtx&&) = delete;

		void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
	};
}; // namespace Wrench

#endif // !VULKAN_CTX_H
